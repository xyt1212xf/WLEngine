require("script/frame/MainFrameInc")
require("script/frame/MsgInc")	
require("script/frame/GameState")
require("script/Role")
require("script/GameplayTags")
require("script/GameAttributeSet")
require("script/actor/BaWangWan/BaWangWan")

SceneUI = setmetatable({}, {__index = GameState:New()})
SceneUI.__index = SceneUI

RealPlayer = nil
RobotPlayer = nil

function SceneUI:MsgProcess(msgId, charCode)
	if 	EGameMessage.Char == msgId or 
	  	EGameMessage.KeyUp == msgId then 
		local k = string.char(charCode)
		self.Roles[1]:MsgProcess(msgId, k)
	end
	return 0
end

function SceneUI:New()
	local sceneUI = self
	return sceneUI
end

function SceneUI:Init()
	math.randomseed(os.time())
	loadScene("resource/Scene/Fight1", false)
	self:setLuaEntityName("SceneUI")
	self:initialise("resource/Gui/Fight1Wnd.gui")
	self:setVisible(true)
	self.Roles = 
	{
		[1] = Role:New(),
	} 
	self.Robot = 
	{
		[1] = BaWangWan:New()
	}
	InitialiseGameplayTags()
	local player = self.Roles[1]
	print(player)
	player:Init("BaWangWan", 150, 480)
	player:SetName("Player1")
	player:SetState(RoleStateEnum.stand)

	local computerActor = self.Robot[1]
	print(computerActor)
	computerActor:InitActor(750, 480, 1)
	RealPlayer = player
	RobotPlayer = computerActor

	player.opponent = RobotPlayer
	computerActor.opponent = RealPlayer
end

function SceneUI:Tick()
	for i = 1, #self.Roles do
		local player = self.Roles[i]
		player:OnTick()
	end
	for i = 1, #self.Robot do 
		local robot = self.Robot[i]
		robot:OnTick()
	end
end

return SceneUI



