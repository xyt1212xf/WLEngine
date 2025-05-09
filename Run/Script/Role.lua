
require("script/RoleConfig")
require("script/State")


Role = {}
Role.__index = Role

function Role:New()
	local role = setmetatable({}, self)
	return role
end

function Role:Init(szName, x, y)
    self.entity = CActorEntity:New()
    self.currentState = -1
    self.stateMap = {}
    self.x = x
    self.y = y
    self.otherPlayer = nil
    self:LoadState(szName)
    self.entity:enterScene()

    self.entity:test(self.entity)
end

function Role:UnInit()
    self.actor:leaveScene()
    self.actor = nil
    self.entity = nil
end

function Role:SetName(name)
    Role.name = name
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
    if EGameMessage.Char == msgId then 
        if key == 'w' or key == 'W'  then
            self:SetState(RoleStateEnum.jump)
	    elseif key == 's' or key == 'S'  then
            self:SetState(RoleStateEnum.crouch)
	    elseif key == 'a' or key == 'A'  then
            self:SetState(RoleStateEnum.back)
	    elseif key == 'd' or key == 'D'  then
            self:SetState(RoleStateEnum.walk)
        elseif key == 'j' or key == 'J'  then
            self:SetState(RoleStateEnum.attack1)
        elseif key == 'k' or key == 'K'  then
            self:SetState(RoleStateEnum.attack2)
	    end
    elseif EGameMessage.KeyUp == msgId then
        local state = self:GetState()
        if state and state:IsInterrupt() then
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



