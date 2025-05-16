require("script/Role")

require("script/actor/BaWangWan/BaWangWanBTree")
require("script/actor/BaWangWan/BaWangWanAttributeSet")

BaWangWan = {}
BaWangWan.__index = BaWangWan
setmetatable(BaWangWan, Role) -- Set Role as the parent

function BaWangWan:New()
	local instance = setmetatable({}, self)
	instance.components = {}
    return instance
end

function BaWangWan:InitActor(x, y, level)
	local robotName = "BaWangWan"
	self:Init(robotName, x, y)
	self:SetName("computerActor->BaWangWan")
	self:SetState(RoleStateEnum.stand)
	self:TurnRound()
	self.DA = require("script/actor/BaWangWan/BaWangWanDA") 
	self.DA:OnGiveAbility()
	self.components.ability = CAbilitySystemComponent:New()
	
	self.btTree = BaWangWanBTree:New()
	self.btTree:Init(robotName, level)
	self.entity:addComponent(self.components.ability)

	self.entity:addBehaviorTree(robotName)
	self.attributeSet = BaWangWanAttributeSet:New()

	self.attributeSet:StartUpData()
end








