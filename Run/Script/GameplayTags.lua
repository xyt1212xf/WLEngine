

function InitialiseGameplayTags()
	InputTag_Move = GameplayTag:New()
	InputTag_Move:setName("InputTag.Move")

	InputTag_LightAttack = GameplayTag:New()
	InputTag_LightAttack:setName("InputTag.LightAttack")

	InputTag_HeavyAttack = GameplayTag:New()
	InputTag_HeavyAttack:setName("InputTag.HeavyAttack")

	Player_Event_Equip_Weapon = GameplayTag:New()
	Player_Event_Equip_Weapon:setName("Player.Event.Equip.Weapon")

	Player_Event_Unequip_Weapon = GameplayTag:New()
	Player_Event_Unequip_Weapon:setName("Player.Event.Unequip.Weapon")

	Player_Event_Rage = GameplayTag:New()
	Player_Event_Rage:setName("Player.Event.Rage")

	Player_Event_Anger = GameplayTag:New()
	Player_Event_Anger:setName("Player.Event.Anger")
end

function UnInitialiseGameplayTags()
	InputTag_Move = nil
	InputTag_LightAttack = nil
	InputTag_HeavyAttack = nil
	Player_Event_Equip_Weapon = nil
	Player_Event_Unequip_Weapon = nil 
	Player_Event_Rage = nil 
	Player_Event_Anger = nil
end



