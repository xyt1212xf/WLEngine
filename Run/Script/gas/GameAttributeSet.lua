
GameplayAttribute = 
{
    
}

GameAttributeSet = {}
GameAttributeSet.CObject = CAttributeSet:New()
GameAttributeSet.__index = GameAttributeSet
GameAttributeSet.xxx = 0
function GameAttributeSet.New()
    local newInstance = {}
    setmetatable(newInstance, GameAttributeSet)
   
    newInstance:Initialise()
    newInstance.CObject = CAttributeSet:New()
    return newInstance
end

function GameAttributeSet:StartUpData()

end

function GameAttributeSet:Initialise()
    self:StartUpData()
end

function GameAttributeSet:preAttributeChange(attribute, newValue)

end

function GameAttributeSet:postAttributeChange(attribute, oldValue, newValue)
    
end






