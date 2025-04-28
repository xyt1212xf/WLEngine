
BaWangWanBTree = {}
BaWangWanBTree.__index = BaWangWanBTree

function BaWangWanBTree:New()
	local newInstance = {}
    setmetatable(newInstance, BaWangWanBTree)
	return newInstance
end

function BaWangWanBTree:Init(robotName, level)
	createBT(robotName)
	if 1 == level then
		local sequenceNode = CSequenceNode:New()
		sequenceNode:addNode(FindNode)
		sequenceNode:addNode(AttackNode)

		local selectorNode = CSelectorNode:New()
		selectorNode:addNode(sequenceNode)
		selectorNode:addNode(DefenseNode)
		selectorNode:addNode(MoveNode)
		selectorNode:addNode(IdleNode)

		setBTreeRootNode(robotName, selectorNode)
	end
end

IdleNode = CTaskNode:New()
IdleNode:callBack("IdleNode")

function IdleNode:Execute()
	if RealPlayer and RobotPlayer then
		local randomInt = math.random(1, 10)
		if (random < 4) then
			local x, y = RealPlayer:GetPosition()
			print("realPlayer x = "..x)	
			x, _ = RobotPlayer:GetPosition()	
			print("robotPlayer x = "..x)	
			return true
		end
	end
	return false
end


FindNode = CTaskNode:New()
FindNode:callBack("FindNode")

function FindNode:Execute()
	if RealPlayer and RobotPlayer then
		local x1 = RealPlayer:GetPosition()
		local x2 = RobotPlayer:GetPosition()	
		print("x1 - x2 = " ..math.abs(x1 - x2) )
		
		if math.abs(x1 - x2) > 500 then
			RobotPlayer:SetState(RoleStateEnum.walk)
			return true
		elseif math.abs(x1 - x2) < 100 then
			RobotPlayer:SetState(RoleStateEnum.back)
			return true
		else
			return false
		end
	end
	return false
end

		


DefenseNode = CTaskNode:New()
DefenseNode:callBack("DefenseNode")

function DefenseNode:Execute()
	if RealPlayer and RobotPlayer then
		local x1 = RealPlayer:GetPosition()
		local x2 = RobotPlayer:GetPosition()	
		if math.abs(x1 - x2) < 50 then
			print("DefenseNode")
			return true
		end
	end
	return false
end


AttackNode = CTaskNode:New()
AttackNode:callBack("AttackNode")

function AttackNode:Execute()
	if RealPlayer and RobotPlayer then

	end
	return false
end


MoveNode = CTaskNode:New()
MoveNode:callBack("MoveNode")

function MoveNode:Execute()
	if RealPlayer and RobotPlayer then		
		local x1 = RealPlayer:GetPosition()
		local x2 = RobotPlayer:GetPosition()	
		print("x1 - x2 = " ..math.abs(x1 - x2) )
		
		if math.abs(x1 - x2) > 500 then
			RobotPlayer:SetState(RoleStateEnum.walk)
			return true
		elseif math.abs(x1 - x2) < 100 then
			RobotPlayer:SetState(RoleStateEnum.back)
			return true
		end	
	end
	return false
end

