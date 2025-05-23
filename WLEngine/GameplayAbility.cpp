#include "GameplayAbility.h"
#include "GameplayAbilityTypes.h"
#include "GameplayAbilitySpec.h"
#include "WLEngine.h"

namespace WL
{
	DefineScriptClass(CGameplayAbility)
	GameplayAbilityActorInfo CGameplayAbility::GetActorInfo() const
	{
		if (nullptr == mCurrentActorInfo)
		{
			return GameplayAbilityActorInfo();
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

	void CGameplayAbility::onGiveAbility(const GameplayAbilityActorInfo* ActorInfo, const SGameplayAbilitySpec& Spec)
	{
		setCurrentActorInfo(Spec.mHandle, ActorInfo);
	}

	void CGameplayAbility::endAbility(const GameplayAbilitySpecHandle Handle, const GameplayAbilityActorInfo* ActorInfo, const SGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
	{

	}

	void CGameplayAbility::setCurrentActorInfo(const GameplayAbilitySpecHandle Handle, const GameplayAbilityActorInfo* ActorInfo) const
	{
		mCurrentActorInfo = ActorInfo;
		mCurrentSpecHandle = Handle;
	}

}