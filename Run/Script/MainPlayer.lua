require("script/RoleHeader")

local MainPlayer = 
{
	CObj = nil,
	name = "Hero",
	hp = 100, 
	maxHp = 100,
	state = RoleStateEnum.stand,
	weapon = 1,
}

gMainPlayer = nil

function MainPlayerUnInit()
	if gMainPlayer then
		gMainPlayer.CObj = nil
		gMainPlayer = nil
	end
end

function MainPlayerInit()
	MainPlayerUnInit()
	gMainPlayer = MainPlayer
	gMainPlayer.CObj = CActorEntity.New()
	gMainPlayer.CObj:setName(gMainPlayer.name)
	print(gMainPlayer.name)
end

function MainPlayer:OnTick()
	
end

function MainPlayer:GetState()

end

return gMainPlayer

