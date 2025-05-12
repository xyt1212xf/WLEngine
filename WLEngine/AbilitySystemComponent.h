#pragma once
#include "Component.h"
#include "ActorEntity.h"
#include "GameplayAbilitySpecHandle.h"
#include "GameplayAbilitySpec.h"
#include "GameplayAbilityTypes.h"

namespace WL
{

	struct GameplayAbilitySpec;
	class CAbilitySystemComponent : public CComponent
	{
	public:
		CAbilitySystemComponent();
		void setOwnerActor(CActorEntity* NewOwnerActor);
		bool tryActivateAbility(GameplayAbilitySpecHandle AbilityToActivate);
		GameplayAbilitySpec* findAbilitySpecFromHandle(GameplayAbilitySpecHandle Handle) const;

		CActorEntity* getOwnerActor() const 
		{
			return mpOwnerActor; 
		}

	protected:
		/** Will be called from GiveAbility or from OnRep. Initializes events (triggers and inputs) with the given ability */
		virtual void onGiveAbility(GameplayAbilitySpec& AbilitySpec);

		/** Will be called from RemoveAbility or from OnRep. Unbinds inputs with the given ability */
		virtual void onRemoveAbility(GameplayAbilitySpec& AbilitySpec);
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
		GameplayAbilitySpecContainer mActivatableAbilities;
		std::vector<GameplayAbilitySpecHandle> mAbilityPendingRemoves;
		std::vector<GameplayAbilitySpec> mAbilityPendingAdds;

	private:
		bool mbAbilityPendingClearAll = false;
		CActorEntity* mpOwnerActor = nullptr;
		INT32	mAbilityScopeLockCount = 0;
	};
}