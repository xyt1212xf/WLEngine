require("script/gas/GameAttributeData")
require("script/gas/GameAttributeSet")  

BaWangWanAttributeSet = setmetatable({}, GameAttributeSet)
BaWangWanAttributeSet.__index = BaWangWanAttributeSet


function BaWangWanAttributeSet:New()
    local newInstance = {}
    setmetatable(newInstance, BaWangWanAttributeSet)
    newInstance.cObject = CAttributeSet:New()
    newInstance.cObject:doBreak()
    self.currentHealth = GameAttributeData:New()
	self.maxHealth = GameAttributeData:New()
	self.currentRage = GameAttributeData:New()
	self.maxRage = GameAttributeData:New()
	self.attackPower = GameAttributeData:New()
	self.defensePower = GameAttributeData:New()

    return newInstance
end

function BaWangWanAttributeSet:StartUpData()
    local Data = BaWangWanStartUpData:New()
   
    self.currentHealth.baseValue = 0
    self.currentHealth.currentValue = 0
    self.currentRage.baseValue = 0
    self.currentRage.currentValue = 0

    self.maxHealth.baseValue = Data.maxHealth[1]
    self.maxHealth.currentValue = Data.maxHealth[1]
    self.maxRage.baseValue = Data.maxRage[1]
    self.maxRage.currentValue = Data.maxRage[1]
    self.attackPower.baseValue = Data.attackPower[1]
    self.attackPower.currentValue = Data.attackPower[1]
    self.defensePower.baseValue = Data.defensePower[1]
    self.defensePower.currentValue = Data.defensePower[1]
end








