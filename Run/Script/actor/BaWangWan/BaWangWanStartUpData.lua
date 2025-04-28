
BaWangWanStartUpData = {}
BaWangWanStartUpData.__index = BaWangWanStartUpData

local function InitData()
	BaWangWanStartUpData.maxHealth = 
	{
		[1] = 100,
		[2] = 20,
		[3] = 30,
		[4] = 40,
		[5] = 50,
	}

	BaWangWanStartUpData.maxRage = 
	{
		[1] = 100,
		[2] = 20,
		[3] = 30,
		[4] = 40,
		[5] = 50,
	}

	BaWangWanStartUpData.attackPower = 
	{
		[1] = 10,
		[2] = 20,
		[3] = 30,
		[4] = 40,
		[5] = 50,
	}

	BaWangWanStartUpData.defensePower = 
	{
		[1] = 0.1,
		[2] = 20,
		[3] = 30,
		[4] = 40,
		[5] = 50,
	}
end

function BaWangWanStartUpData:New()
	local newInstance = deepCopy(self)
	return newInstance
end



