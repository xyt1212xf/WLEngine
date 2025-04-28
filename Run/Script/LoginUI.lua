require("script/frame/MainFrameInc")
require("script/frame/MsgInc")	
require("script/frame/GameState")
require("script/MainPlayer")

LoginUI = setmetatable({}, {__index = GameState:New()})
LoginUI.__index = LoginUI

function LoginUI:MsgProcess(msgId, id)
	local startID = 1
	local exitID = 2
	local selectID = 3
	local labelID = 4

	if EGameMessage.LMouseUp == msgId then 
		if id == startID then
			LoginUI:setVisible(false)	
			gApp:ChangeGameState(EGameState.eToGameMain)
			self:leaveScene()
		elseif id == selectID then
			LoginUI:setVisible(false)	
			gApp:ChangeGameState(EGameState.eToSelectChar)
		elseif id == exitID then
			LoginUI:setVisible(false)	
			exit()
		end
	end
	return 0
end

function LoginUI:New()
	local login = setmetatable({}, LoginUI)
	login.__index = login 
	return login
end

function LoginUI:Init()
	loadScene("resource/Scene/Login", false)
	self:setLuaEntityName("LoginUI")
	self:initialise("resource/Gui/LoginWnd.gui")
	self:setVisible(true)
	self:setChildText(3, "T")
end

function LoginUI:Tick()
end

return LoginUI



