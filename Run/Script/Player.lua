require("script/frame/MainFrameInc")
require("script/Role")
Player = 
{
}

function Player:New()
	local role = Role:New()
	local player = {}
	setmetatable(player, {__index = role})
	player.__index = player
	return player
end