

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

	Player_Event_Vertigo = GameplayTag:New()
	Player_Event_Vertigo:setName("Player.Event.Vertigo")

	Player_Event_Anger = GameplayTag:New()
	Player_Event_Anger:setName("Player.Event.Anger")
end

function UnInitialiseGameplayTags()
	InputTag_Move = nil
	InputTag_LightAttack_Weapon = nil
	InputTag_HeavyAttack_Weapon = nil
	Player_Event_Equip_Weapon = nil
	Player_Event_Unequip_Weapon = nil 
	Player_Event_Vertigo = nil 
	Player_Event_Anger = nil
end
