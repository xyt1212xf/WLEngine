require("script/frame/MainFrameInc")
require("script/LoginUI")
require("script/SeleceRoleUI")
require("script/SceneUI")
require("script/GameMainUI")
GameApp = 
{
	State = EGameState.eToLogin
}
gApp = GameApp

local gameState = nil
function start()
	initBT()
	MainPlayerInit()
	gApp:ChangeGameState(EGameState.eToLogin)
end

function tick()
	gameState:Tick()	
end

function GameApp:ChangeGameState(state)
	self.State = state
	if EGameState.eToLogin == self.State then
		gameState = LoginUI:New()
	elseif EGameState.eToSelectChar == self.State then
		gameState = SeleceRoleUI:New()
	elseif EGameState.eToGameScene == self.State then
		gameState = SceneUI:New()
	elseif EGameState.eToGameMain == self.State then
		gameState = GameMainUI:New()
	end
	gameState:Init()
end






