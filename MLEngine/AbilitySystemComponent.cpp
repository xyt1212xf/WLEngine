#include "AbilitySystemComponent.h"
#include "GameplayAbility.h"
#include "WLEngine.h"

namespace WL
{
    DefineScriptClass(CAbilitySystemComponent)
	CAbilitySystemComponent::CAbilitySystemComponent()
	{
        mComponentName = "AbilitySystemComponent";
	}

	void CAbilitySystemComponent::registerClass()
	{
		CRegisterClass<CAbilitySystemComponent>::registerClass(GEngine->getLuaState());
	}


	void CAbilitySystemComponent::registerMember()
	{
	}

	void CAbilitySystemComponent::setOwnerActor(CActorEntity* NewOwnerActor)
    {
        if (mpOwnerActor != NewOwnerActor)
        {
            WL_INCREASE(NewOwnerActor);
            WL_DECREASE(mpOwnerActor);
            mpOwnerActor = NewOwnerActor;
            WL_INCREASE(mpOwnerActor);
        }
    }

    bool CAbilitySystemComponent::tryActivateAbility(SGameplayAbilitySpecHandle AbilityToActivate)
    {
        SGameplayTagContainer FailureTags;
        SGameplayAbilitySpec* Spec = findAbilitySpecFromHandle(AbilityToActivate);
        if (!Spec)
        {
            return false;
        }

        CGameplayAbility* Ability = Spec->mpAbility;

        if (nullptr == Ability)
        {
            return false;
        }
        const SGameplayAbilityActorInfo* ActorInfo = mpAbilityActorInfo;
        if (nullptr == ActorInfo)
        {
            return false;
        }

        return internalTryActivateAbility(AbilityToActivate);;
    }

    SGameplayAbilitySpec* CAbilitySystemComponent::findAbilitySpecFromHandle(SGameplayAbilitySpecHandle Handle) const
    {
        return nullptr;
    }


    void CAbilitySystemComponent::onGiveAbility(SGameplayAbilitySpec& AbilitySpec)
    {
        if (nullptr != AbilitySpec.mpAbility)
        {
            AbilitySpec.mpAbility->onGiveAbility(mpAbilityActorInfo, AbilitySpec);
        }
    }

	void CAbilitySystemComponent::onRemoveAbility(SGameplayAbilitySpec& AbilitySpec)
	{

	}

	void CAbilitySystemComponent::incrementAbilityListLock()
	{
        ++mAbilityScopeLockCount;
	}

	void CAbilitySystemComponent::decrementAbilityListLock()
	{
        if (--mAbilityScopeLockCount == 0)
        {
            if (mbAbilityPendingClearAll)
            {
                clearAllAbilities();
                mAbilityPendingAdds.clear();
				// Pending removes are no longer relevant since all abilities have been removed
                mAbilityPendingRemoves.clear();

            }
			else if (mAbilityPendingAdds.size() > 0 || mAbilityPendingRemoves.size() > 0)
			{
				//AbilityListLockActiveChange ActiveChange(*this, AbilityPendingAdds, AbilityPendingRemoves);

				//for (FGameplayAbilitySpec& Spec : ActiveChange.Adds)
				//{
				//	if (Spec.bActivateOnce)
				//	{
				//		GiveAbilityAndActivateOnce(Spec, Spec.GameplayEventData.Get());
				//	}
				//	else
				//	{
				//		GiveAbility(Spec);
				//	}
				//}

				//for (FGameplayAbilitySpecHandle& Handle : ActiveChange.Removes)
				//{
				//	ClearAbility(Handle);
				//}
			}
        }
	}

	bool CAbilitySystemComponent::internalTryActivateAbility(SGameplayAbilitySpecHandle AbilityToActivate)
    {
        if (AbilityToActivate.isValid() == false)
        {
            return false;
        }
        return true;
    }

	void CAbilitySystemComponent::clearAbility(const SGameplayAbilitySpecHandle& Handle)
	{
        auto iter = mActivatableAbilities.mItems.begin();
        while (iter != mActivatableAbilities.mItems.end())
        {   
			if ((*iter).mHandle.isValid())
			{
				if ((*iter).mHandle == Handle)
				{
					if (mAbilityScopeLockCount > 0)
					{
						if (false == (*iter).mPendingRemove)
						{
                            (*iter).mPendingRemove = true;
							mAbilityPendingRemoves.emplace_back(Handle);
						}
					}
					else
					{
						onRemoveAbility((*iter));
                        iter = mActivatableAbilities.mItems.erase(iter);
                        continue;
					}
				}
			}
            ++iter;
        }

	}

	void CAbilitySystemComponent::clearAllAbilities()
	{

	}

}