

EGameState = {}

EGameState.eExitGame		= 0
EGameState.eToSelectChar	= 1
EGameState.eToLogin			= 2
EGameState.eToGameMain		= 3
EGameState.eToGetBackRole	= 4
EGameState.eToNewRoleGameMain = 5
EGameState.eToAgreement = 6
EGameState.eToGameScene = 7

GameState = setmetatable({}, {__index = CGuiEntity.New()})
GameState.__index = GameState

function GameState:New()
	local newObj = setmetatable({}, self)
	newObj.__index = newObj
	return newObj
end

function GameState:Main()
	self.run = true
	local startTime = os.clock()
	while self.run and EGameState.eExitGame ~= gApp.State do
		local currentTime = os.clock()
		local elapsedTime = (currentTime - startTime) * 1000 -- ×ª»»ÎªºÁÃë
		if elapsedTime >= 10 then	
			startTime = currentTime
			self:Tick()
		end
	end
end

function GameState:Tick()

end

function GameState:Exit()
	self.Run = false
	gApp.State = EGameState.eExitGame 
end

return GameState


