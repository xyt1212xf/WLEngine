#pragma once
#include "Component.h"
#include "ActorEntity.h"
#include "GameplayAbilitySpecHandle.h"
#include "GameplayAbilitySpec.h"
#include "GameplayAbilityTypes.h"
#include "ScriptEntity.h"

namespace WL
{
	struct SGameplayAbilitySpec;
	class CAbilitySystemComponent : public CComponent, public CScriptEntity
	{
		DeclareScriptClass(CAbilitySystemComponent);
	public:
		CAbilitySystemComponent();
		void setOwnerActor(CActorEntity* NewOwnerActor);
		bool tryActivateAbility(GameplayAbilitySpecHandle AbilityToActivate);
		SGameplayAbilitySpec* findAbilitySpecFromHandle(GameplayAbilitySpecHandle Handle) const;

		CActorEntity* getOwnerActor() const 
		{
			return mpOwnerActor; 
		}

	protected:
		/** Will be called from GiveAbility or from OnRep. Initializes events (triggers and inputs) with the given ability */
		virtual void onGiveAbility(SGameplayAbilitySpec& AbilitySpec);

		/** Will be called from RemoveAbility or from OnRep. Unbinds inputs with the given ability */
		virtual void onRemoveAbility(SGameplayAbilitySpec& AbilitySpec);
		void incrementAbilityListLock();
		void decrementAbilityListLock();

	private:
		/** Attempts to activate the given ability, will only work if called from the correct client/server context */
		bool internalTryActivateAbility(GameplayAbilitySpecHandle AbilityToActivate);
		void clearAbility(const GameplayAbilitySpecHandle& Handle);
		void clearAllAbilities();

	public:
		GameplayAbilityActorInfo*	mpAbilityActorInfo = nullptr;

	protected:
		SGameplayAbilitySpecContainer mActivatableAbilities;
		std::vector<GameplayAbilitySpecHandle> mAbilityPendingRemoves;
		std::vector<SGameplayAbilitySpec> mAbilityPendingAdds;

	private:
		bool mbAbilityPendingClearAll = false;
		CActorEntity* mpOwnerActor = nullptr;
		INT32	mAbilityScopeLockCount = 0;
	};
}