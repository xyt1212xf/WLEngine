require("script/frame/MainFrameInc")
require("script/frame/MsgInc")	
require("script/frame/GameState")

SeleceRoleUI = setmetatable({}, {__index = GameState:New()})
SeleceRoleUI.__index = SeleceRoleUI

function SeleceRoleUI:MsgProcess(msgId, id)
	return 0
end

function SeleceRoleUI:New()
	print("SelectRoleNew")
	local selectRole = setmetatable({}, SeleceRoleUI)
	selectRole.__index = selectRole 
	return selectRole
end

function SeleceRoleUI:Init()
	gApp:ChangeGameState(EGameState.eToGameScene)
end

function SeleceRoleUI:Tick()
	--print("tick")
end

return SeleceRoleUI



