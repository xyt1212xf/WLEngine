--[[
print("Lua version:", _VERSION)

local socket = dofile("E:/WLEngine/Run/Script/socket.lua")
print("luasocket �汾:", socket._VERSION)

local lpd = dofile("E:/WLEngine/Run/Script/LuaPanda.lua")
print("lpd isLoad successed:", tostring(lpd ~= nil))
lpd.start("127.0.0.1", 8818)
]]

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
function ScriptStart()
	initialiseGameplayTags("GameplayTags.json")
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


function ScriptEnd()
	unInitialiseGameplayTags()
end



