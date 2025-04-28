

State = {}
State.__index = State

function State:New()
	local state = setmetatable({}, State)
    state.__index = state 
	return state
end 

function State:Init(role, stateCfg)
    self.entity = CGuiEntity.New()
    self.entity:loadGui("resource/Gui/RoleWnd.gui")
    self.entity:setAnimation(stateCfg.frameCount.." "..stateCfg.frameName)
    self.entity:setAniLoop(stateCfg.isLoop)

    self.cfg = stateCfg
    self.role = role
    local x, y = role:GetPosition()
    self.entity:setX(x)
    self.entity:setY(y)

    self.entity:setAniSpeed(self.cfg.speed)
end

function State:UnInit()
    self.entity = nil
end

function State:Enter()
    print("State Enter = "..self.cfg.id)
    self.entity:enterScene()
    self.entity:setVisible(true)
    self.entity:setCurrentFrame(0)
    local x, y = self.role:GetPosition()
    self.entity:setX(x)
    self.entity:setY(y)

end

function State:Leave()
    self.entity:leaveScene()
    self.entity:setVisible(false)
    self.entity:setX(self.entity:getX())
    self.entity:setY(self.entity:getY())
    print("State Leave = "..self.cfg.id)
end

function State:OnTick()
    local nSpeedX = self.cfg.moveSpeed.x
    local nSpeedY = self.cfg.moveSpeed.y
    if self.entity:isChildXYAxisSwap(100) then
        nSpeedX = -nSpeedX;
    end

    local x = self.entity:getX() + nSpeedX
    local y = self.entity:getY() + nSpeedY

    self.entity:setX(x)
    self.entity:setY(y)
    self.role:SetPosition(x, y)
end

function State:GetActor()
    return self.entity
end

function State:IsInterrupt()
    if self.entity:getCurrentFrame() < self.cfg.interrupt then
        return true 
    else 
        return false
    end
end
