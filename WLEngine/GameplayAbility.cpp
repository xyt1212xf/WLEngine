#include "GameplayAbility.h"
#include "GameplayAbilityTypes.h"
#include "GameplayAbilitySpec.h"
#include "WLEngine.h"

namespace WL
{
	DefineScriptClass(CGameplayAbility)
	SGameplayAbilityActorInfo CGameplayAbility::GetActorInfo() const
	{
		if (nullptr == mCurrentActorInfo)
		{
			return SGameplayAbilityActorInfo();
		}
		return *mCurrentActorInfo;
	}

	void CGameplayAbility::registerClass()
	{
		CRegisterClass<CGameplayAbility>::registerClass(GEngine->getLuaState());
	}

	void CGameplayAbility::registerMember()
	{
	}

	void CGameplayAbility::onGiveAbility(const SGameplayAbilityActorInfo* ActorInfo, const SGameplayAbilitySpec& Spec)
	{
		setCurrentActorInfo(Spec.mHandle, ActorInfo);
	}

	void CGameplayAbility::endAbility(const SGameplayAbilitySpecHandle Handle, const SGameplayAbilityActorInfo* ActorInfo, const SGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
	{

	}

	void CGameplayAbility::setCurrentActorInfo(const SGameplayAbilitySpecHandle Handle, const SGameplayAbilityActorInfo* ActorInfo) const
	{
		mCurrentActorInfo = ActorInfo;
		mCurrentSpecHandle = Handle;
	}

}