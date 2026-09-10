#pragma once
#include "Common.h"
#include "Refcount.h"
#include "GameplayTagContainer.h"

namespace WL
{
	struct SGameplayTagNode : public CRefcount
	{
		SGameplayTagNode() = default;
		SGameplayTagNode(const std::string& InTag, const std::string& InFullTag, SGameplayTagNode* pParent);

		/** Returns a correctly constructed container with only this tag, useful for doing container queries */
		INLINE const SGameplayTagContainer& getSingleTagContainer() const
		{
			return mCompleteTagWithParents; 
		}
		
		INLINE const SGameplayTag& getCompleteTag() const
		{
			return mCompleteTagWithParents.num() > 0 ? mCompleteTagWithParents.mGameplayTags[0] : SGameplayTag::EmptyTag; 
		}
		INLINE const std::string& getSimpleTagName() const
		{
			return mTagName; 
		}
	
		std::string mTagName = "";
		std::string mFullTagName = "";
		SGameplayTagContainer mCompleteTagWithParents;
		std::vector<SGameplayTagNode*>	mChildTags;
		SGameplayTagNode* mParentNodePtr = nullptr;
		UINT16 mNetIndex = 0xFFFF;
	};
}

