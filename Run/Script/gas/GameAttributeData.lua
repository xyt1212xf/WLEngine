GameAttributeData = {}
GameAttributeData.__index = GameAttributeData

function GameAttributeData:New()
	local newInstance = {}
	setmetatable(newInstance, GameAttributeData)
	newInstance:Initialise()
	return newInstance
end

function GameAttributeData:Initialise()
	self.baseValue = 0
	self.currentValue = 0
end




