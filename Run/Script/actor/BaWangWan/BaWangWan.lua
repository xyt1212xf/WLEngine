require("script/Role")
require("script/actor/BaWangWan/BaWangWanBTree")
require("script/actor/BaWangWan/BaWangWanAttributeSet")

BaWangWan = setmetatable({}, Role)
BaWangWan.__index = BaWangWan

function BaWangWan:New()

	local newInstance = {}
    setmetatable(newInstance, BaWangWan)
	return newInstance
end

function BaWangWan:InitActor(x, y, level)
	local xx = GameplayTag:New()
	xx:setName("gametag")
	xx:getName()
	xx = nil
	local robotName = "BaWangWan"
	self:Init(robotName, x, y)
	self:SetName("computerActor->BaWangWan")
	self:SetState(RoleStateEnum.stand)
	self:TurnRound()
	self.btTree = BaWangWanBTree:New()
	self.btTree:Init(robotName, level)
	self.entity:addBehaviorTree(robotName)
	
	self.attribute = BaWangWanAttributeSet:New()
	self.attribute:StartUpData()
end








