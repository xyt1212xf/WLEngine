#pragma once

#include "Refcount.h"
namespace WL
{
	struct GameplayAbilitySpecHandle : public CRefcount
	{
		/** True if GenerateNewHandle was called on this handle */
		bool isValid() const
		{
			return mHandle != INDEX_NONE;
		}

		bool operator==(const GameplayAbilitySpecHandle& Other) const
		{
			return mHandle == Other.mHandle;
		}

		bool operator!=(const GameplayAbilitySpecHandle& Other) const
		{
			return mHandle != Other.mHandle;
		}

		INT32 mHandle = -1;
	};
}