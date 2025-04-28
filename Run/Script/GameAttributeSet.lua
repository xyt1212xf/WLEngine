
GameAttributeSet = {}
GameAttributeSet.__index = GameAttributeSet

GameAttributeSet.cObject = CAttributeSet:New()
function GameAttributeSet.New()
    local newInstance = {}
    setmetatable(newInstance, GameAttributeSet)
    newInstance:Initialise()
    newInstance.cObject = CAttributeSet:New()
    newInstance.cObject:doBreak()
    return newInstance
end

function GameAttributeSet:StartUpData()
        
end

function GameAttributeSet:Initialise()

end







