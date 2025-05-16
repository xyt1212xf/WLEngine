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

function BaWangWan:InitActorDA()
	self.DA = require("script/actor/BaWangWan/BaWangWanDA") 
	self.DA:OnGiveAbility()
	self.DA:ReactiveAbility()
end

function BaWangWan:InitActorBehaviorTree(robotName, level)
	self.btTree = BaWangWanBTree:New()
	self.btTree:Init(robotName, level)
	self.entity:addBehaviorTree(robotName)
end

function BaWangWan:InitActorComponents()
	self.components.ability = CAbilitySystemComponent:New()
	self.entity:addComponent(self.components.ability)
end

function BaWangWan:InitActorAttributes()
	self.attributeSet = BaWangWanAttributeSet:New()
	self.attributeSet:StartUpData()
end

function BaWangWan:InitActor(x, y, level)
	local robotName = "BaWangWan"
	self:Init(robotName, x, y)
	self:SetName("computerActor->BaWangWan")
	self:SetState(RoleStateEnum.stand)
	self:TurnRound()

	self:InitActorDA()
	self:InitActorBehaviorTree(robotName, level)
	self:InitActorComponents()
	self:InitActorAttributes()
end








