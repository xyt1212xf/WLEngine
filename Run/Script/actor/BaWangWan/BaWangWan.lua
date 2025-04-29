require("script/Role")
require("script/actor/BaWangWan/BaWangWanBTree")
require("script/actor/BaWangWan/BaWangWanAttributeSet")

BaWangWan = {}
BaWangWan.__index = BaWangWan
setmetatable(BaWangWan, Role) -- Set Role as the parent

function BaWangWan:New()
	local instance = setmetatable({}, self)
    return instance
end

function BaWangWan:InitActor(x, y, level)
	local robotName = "BaWangWan"
	self:Init(robotName, x, y)
	self:SetName("computerActor->BaWangWan")
	self:SetState(RoleStateEnum.stand)
	self:TurnRound()

	self.btTree = BaWangWanBTree:New()
	self.btTree:Init(robotName, level)

	self.entity:addBehaviorTree(robotName)

	self.attributeSet = BaWangWanAttributeSet:New()

	self.attributeSet:StartUpData()

end








