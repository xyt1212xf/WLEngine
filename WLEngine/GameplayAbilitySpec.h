#pragma once 
#include "Refcount.h"
#include "GameplayAbilitySpecHandle.h"
#include "GameplayTagContainer.h"

namespace WL
{
	class CGameplayAbility;

	struct  SGameplayAbilityActivationInfo
	{
	};

	struct SGameplayAbilitySpec final : public CRefcount
	{
		~SGameplayAbilitySpec() = default;
		CGameplayAbility* mpAbility = nullptr;


		SGameplayAbilitySpecHandle mHandle;
		SGameplayAbilityActivationInfo	mActivationInfo;	
		SGameplayTagContainer mDynamicAbilityTags;
		INT32 mLevel;

		UINT8 mPendingRemove : 1;
	};

	struct SGameplayAbilitySpecContainer
	{
		std::vector<SGameplayAbilitySpec> mItems;
	};
}