#pragma once
#include "GameplayAbilitySpecHandle.h"
#include "GameplayAbilitySpec.h"
#include "ScriptEntity.h"

namespace WL
{
	struct SGameplayAbilitySpec;
	struct SGameplayAbilityActorInfo;
	class CGameplayAbility : public CScriptEntity 
	{
		DeclareScriptClass(CGameplayAbility);
		friend class CAbilitySystemComponent;
	public:

		SGameplayAbilityActorInfo GetActorInfo() const;

		virtual void onGiveAbility(const SGameplayAbilityActorInfo* ActorInfo, const SGameplayAbilitySpec& Spec);
		/** Native function, called if an ability ends normally or abnormally. If bReplicate is set to true, try to replicate the ending to the client/server */
		virtual void endAbility(const SGameplayAbilitySpecHandle Handle, const SGameplayAbilityActorInfo* ActorInfo, const SGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled);

		/** Modifies actor info, only safe on instanced abilities */
		virtual void setCurrentActorInfo(const SGameplayAbilitySpecHandle Handle, const SGameplayAbilityActorInfo* ActorInfo) const;


	public:
		mutable const SGameplayAbilityActorInfo* mCurrentActorInfo = nullptr;

		/** For instanced abilities */
		mutable SGameplayAbilitySpecHandle mCurrentSpecHandle;
	};
}