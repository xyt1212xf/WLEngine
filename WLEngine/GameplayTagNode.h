#pragma once
#include "Common.h"
#include "Refcount.h"
#include "GameplayTagContainer.h"

namespace WL
{
	struct GameplayTagNode : public CRefcount
	{
		GameplayTagNode() = default;
		GameplayTagNode(const std::string& InTag, const std::string& InFullTag, GameplayTagNode* pParent);

		/** Returns a correctly constructed container with only this tag, useful for doing container queries */
		INLINE const GameplayTagContainer& getSingleTagContainer() const
		{
			return mCompleteTagWithParents; 
		}
		
		INLINE const GameplayTag& getCompleteTag() const
		{
			return mCompleteTagWithParents.num() > 0 ? mCompleteTagWithParents.mGameplayTags[0] : GameplayTag::EmptyTag; 
		}
		INLINE const std::string& getSimpleTagName() const
		{
			return mTagName; 
		}
	
		std::string mTagName = "";
		std::string mFullTagName = "";
		GameplayTagContainer mCompleteTagWithParents;
		std::vector<GameplayTagNode*>	mChildTags;
		GameplayTagNode* mParentNodePtr = nullptr;
		UINT16 mNetIndex = 0xFFFF;
	};
}

