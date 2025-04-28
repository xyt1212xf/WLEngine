require("script/frame/MainFrameInc")
require("script/frame/MsgInc")	
require("script/frame/GameState")
require("script/MainPlayer")

GameMainUI = setmetatable({}, {__index = GameState:New()})
GameMainUI.__index = GameMainUI

function GameMainUI:MsgProcess(msgId, id)
	return 0
end

function GameMainUI:New()
	local gameMainUI = setmetatable({}, GameMainUI)
	gameMainUI .__index = gameMainUI  
	return gameMainUI 
end

function GameMainUI:Init()
	loadScene("resource/Scene/GameApp", true)
	self:setLuaEntityName("GameMainUI")
	self:setVisible(true)
end

function GameMainUI:Tick()
end

return GameMainUI



