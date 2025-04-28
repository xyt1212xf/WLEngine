#pragma once 
#include "Refcount.h"
#include "GameplayAbilitySpecHandle.h"
#include "GameplayTagContainer.h"

namespace WL
{
	class CGameplayAbility;

	struct  GameplayAbilityActivationInfo
	{
	};

	struct GameplayAbilitySpec final : public CRefcount
	{
		~GameplayAbilitySpec() = default;
		CGameplayAbility* mpAbility = nullptr;


		GameplayAbilitySpecHandle mHandle;
		GameplayAbilityActivationInfo	mActivationInfo;	
		GameplayTagContainer mDynamicAbilityTags;
		INT32 mLevel;

		UINT8 mPendingRemove : 1;
	};

	struct GameplayAbilitySpecContainer
	{
		std::vector<GameplayAbilitySpec> mItems;
	};
}