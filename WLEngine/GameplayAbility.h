#pragma once
#include "GameplayAbilitySpecHandle.h"
#include "GameplayAbilitySpec.h"
#include "ScriptEntity.h"

namespace WL
{
	struct GameplayAbilitySpec;
	struct GameplayAbilityActorInfo;
	class CGameplayAbility : public CScriptEntity 
	{
		DeclareScriptClass(CGameplayAbility);
		friend class CAbilitySystemComponent;
	public:

		GameplayAbilityActorInfo GetActorInfo() const;

		virtual void onGiveAbility(const GameplayAbilityActorInfo* ActorInfo, const GameplayAbilitySpec& Spec);
		/** Native function, called if an ability ends normally or abnormally. If bReplicate is set to true, try to replicate the ending to the client/server */
		virtual void endAbility(const GameplayAbilitySpecHandle Handle, const GameplayAbilityActorInfo* ActorInfo, const GameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled);

		/** Modifies actor info, only safe on instanced abilities */
		virtual void setCurrentActorInfo(const GameplayAbilitySpecHandle Handle, const GameplayAbilityActorInfo* ActorInfo) const;


	public:
		mutable const GameplayAbilityActorInfo* mCurrentActorInfo = nullptr;

		/** For instanced abilities */
		mutable GameplayAbilitySpecHandle mCurrentSpecHandle;
	};
}