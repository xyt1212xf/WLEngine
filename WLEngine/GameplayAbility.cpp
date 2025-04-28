#include "GameplayAbility.h"
#include "GameplayAbilityTypes.h"
#include "GameplayAbilitySpec.h"
namespace WL
{

	GameplayAbilityActorInfo CGameplayAbility::GetActorInfo() const
	{
		if (nullptr == mCurrentActorInfo)
		{
			return GameplayAbilityActorInfo();
		}
		return *mCurrentActorInfo;
	}

	void CGameplayAbility::onGiveAbility(const GameplayAbilityActorInfo* ActorInfo, const GameplayAbilitySpec& Spec)
	{
		setCurrentActorInfo(Spec.mHandle, ActorInfo);
	}

	void CGameplayAbility::endAbility(const GameplayAbilitySpecHandle Handle, const GameplayAbilityActorInfo* ActorInfo, const GameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
	{

	}

	void CGameplayAbility::setCurrentActorInfo(const GameplayAbilitySpecHandle Handle, const GameplayAbilityActorInfo* ActorInfo) const
	{
		mCurrentActorInfo = ActorInfo;
		mCurrentSpecHandle = Handle;
	}

}