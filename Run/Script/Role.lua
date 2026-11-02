
require("script/RoleConfig")
require("script/State")


Role = {}
Role.__index = Role

-- 重力系统参数（以“每帧”为单位，可按手感调）
local JUMP_INIT_SPEED = -6   -- 起跳初速度，负值=向上
local GRAVITY = 0.3          -- 重力加速度，正值=向下
local JUMP_XSPEED = 4        -- 前/后跳的水平速度（受朝向翻转影响）

function Role:New()
	local role = setmetatable({}, self)
	return role
end

function Role:Init(szName, x, y)
    self.entity = CActorEntity:New()
    self.currentState = -1
    self.currentStateEnum = -1
    self.stateMap = {}
    self.x = x
    self.y = y
    self.vy = 0                 -- 垂直速度
    self.vx = 0                 -- 水平速度（前/后跳用）
    self.groundY = y            -- 地面基准线（初始位置即地面）
    self.heldKeys = {}          -- 当前按住的按键集合（用于区分“首次按下”与“系统重复”）
    self.lastTapTime = {}        -- 各方向键上次按下时间（os.clock 秒），用于双击检测
    self.otherPlayer = nil
    self:LoadState(szName)
    self.entity:enterScene()
end

function Role:UnInit()
    self.entity:leaveScene()
    self.components = nil
	self.btTree = nil
    self.attributeSet = nil
    self.entity = nil
end

function Role:SetName(name)
    self.name = name
    self.entity:setName(name)
end

function Role:AddComponent(szName)
	self.entity:addComponent(szName)
end

function Role:SetState(index)
    if index == self.currentStateEnum then
        return
    end
    local oldState = self.stateMap[self.currentStateEnum] 
    local newState = self.stateMap[index]
    if oldState then
        oldState:Leave()
        self.currentState = nil
        self.currentStateEnum = -1
    end
  
    if newState then
        newState:Enter()
        self.currentState = newState
        self.currentStateEnum = index
    end
end

function Role:GetState()
    return self.currentState
end

function Role:GetStateEnum()
    return self.currentStateEnum
end

-- 根据当前按住的按键决定起跳方向（仅在地面时调用）
function Role:TryStartJump()
    if self.currentStateEnum == RoleStateEnum.jump or self.currentStateEnum == RoleStateEnum.down then
        return false
    end
    self.vy = JUMP_INIT_SPEED
    if self.heldKeys['d'] then
        self.vx = JUMP_XSPEED
    elseif self.heldKeys['a'] then
        self.vx = -JUMP_XSPEED
    else
        self.vx = 0
    end
    self:SetState(RoleStateEnum.jump)
    return true
end


function Role:LoadState(szName)
    if szName then
        if Roles[szName] then
            for k, v in pairs(RoleStateEnum) do
                local stateCfg = Roles[szName].state[v]
                if stateCfg then
                    local state = State:New() 
                    state:Init(self, stateCfg) 
                    self.stateMap[stateCfg.id] = state         
                end
            end
        end
    end
end

function Role:OnTick()

    if self.currentState then
        self.currentState:OnTick()
    end

    -- 重力系统：跳/落状态做抛物线运动，落回地面即站定（避免越落越低）
    local enum = self.currentStateEnum
    if enum == RoleStateEnum.jump or enum == RoleStateEnum.down then
        self.vy = (self.vy or 0) + GRAVITY          -- 每帧累加重力加速度
        local y = self.y + self.vy

        -- 水平跳跃速度（受朝向翻转影响，与 walk/back 一致）
        local actor = self.currentState and self.currentState:GetActor()
        local vx = self.vx or 0
        if actor and actor:isChildXYAxisSwap(100) then
            vx = -vx
        end
        local x = self.x + vx

        if y >= self.groundY then
            -- 落地：钳回地面并清零速度
            y = self.groundY
            self.vy = 0
            self.vx = 0
            if actor then actor:setX(x); actor:setY(y) end
            self:SetPosition(x, y)
            -- 落地后：仅 w 触发连跳；若仍按住方向键则恢复走路；否则站定
            if self.heldKeys['w'] then
                self:TryStartJump()
            elseif self.heldKeys['a'] then
                self:SetState(RoleStateEnum.back)
            elseif self.heldKeys['d'] then
                self:SetState(RoleStateEnum.walk)
            else
                self:SetState(RoleStateEnum.stand)
            end
        else
            -- 空中：上升段转下落时切换“下落”动画
            if actor then actor:setX(x); actor:setY(y) end
            self:SetPosition(x, y)
            if enum == RoleStateEnum.jump and self.vy > 0 then
                self:SetState(RoleStateEnum.down)
            end
        end
    end

    self:CheckXYAxisSwap() 
end

function Role:CheckXYAxisSwap()
  if self.opponent then
        local actor = self.currentState:GetActor()
        if actor then
            local opponent = self.opponent:GetPosition() 
            if self.x < opponent then
                actor:setChildXYAxisSwap(100, false) 
            else
                actor:setChildXYAxisSwap(100, true)  
            end
        end
    end
end

function Role:MsgProcess(msgId, key)
    local k = (key and string.lower(key)) or key
    if EGameMessage.Char == msgId then
        -- 区分“真实按下”与“系统按键自动重复”：Char 首次到达时 heldKeys[k] 为 nil，
        -- 重复到达时已被置为 true。仅用 fresh 判定双击，不影响移动（移动每帧都允许）。
        local fresh = not self.heldKeys[k]
        self.heldKeys[k] = true

        local enum = self.currentStateEnum
        local airborne = (enum == RoleStateEnum.jump or enum == RoleStateEnum.down)

        if k == 'w' then
            if not airborne then self:TryStartJump() end
        elseif k == 'a' then
            -- 地面：左/后走（不跳）；空中：仅纯竖直跳(vx==0)可追加方向，已定向的跳跃不可反向
            if airborne then
                if self.vx == 0 then self.vx = -JUMP_XSPEED end
            else
                self:SetState(RoleStateEnum.back)
            end
        elseif k == 'd' then
            -- 地面：右/前走；空中：同上
            if airborne then
                if self.vx == 0 then self.vx = JUMP_XSPEED end
            else
                -- 不依赖 KeyUp/fresh：当前在走路且 0.4s 内又收到一次 d -> 奔跑。
                -- （之前依赖 fresh 会因 KeyUp 不可靠而永远进不了 run）
                local now = os.clock()
                local dt = self.lastTapTime['d'] and (now - self.lastTapTime['d']) or 999
                self.lastTapTime['d'] = now
                if dt > 0.04 and dt <= 0.4 and self.currentStateEnum == RoleStateEnum.walk then
                    print(string.format("[DBG] enter RUN dt=%.3f loaded=%s", dt,
                        tostring(self.stateMap[RoleStateEnum.run] ~= nil)))
                    self:SetState(RoleStateEnum.run)
                else
                    self:SetState(RoleStateEnum.walk)
                end
            end
        elseif k == 's' then
            self:SetState(RoleStateEnum.crouch)
        elseif k == 'j' then
            self:SetState(RoleStateEnum.attack1)
        elseif k == 'k' then
            self:SetState(RoleStateEnum.attack2)
        end
    elseif EGameMessage.KeyUp == msgId then
        self.heldKeys[k] = nil
        local state = self:GetState()
        local enum = self:GetStateEnum()
        -- 跳/落在空中由重力系统接管，松手不做特殊处理
        if enum ~= RoleStateEnum.jump and enum ~= RoleStateEnum.down and state and state:IsInterrupt() then
            self:SetState(RoleStateEnum.stand)
        end
    end
end

function Role:SetPosition(x, y)
    self.x = x
    self.y = y
end

function Role:GetPosition()
	return self.x, self.y
end

function Role:TurnRound()
    
    if self.currentState then
        local actor = self.currentState:GetActor()
        if actor then
            actor:setChildXYAxisSwap(100, true)   
        end
    end
end



