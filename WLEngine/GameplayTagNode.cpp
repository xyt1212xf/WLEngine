#include "GameplayTagNode.h"
namespace WL
{
    GameplayTagNode::GameplayTagNode(const std::string& InTag, const std::string& InFullTag, GameplayTagNode* ParentPtr)
    {
        mTagName = InTag;
        mParentNodePtr = ParentPtr;
        mCompleteTagWithParents.mGameplayTags.emplace_back(GameplayTag(InFullTag));
        if (nullptr != ParentPtr && ParentPtr->mTagName.length() > 1)
        {
            //ParentPtr->mCompleteTagWithParents.mGameplayTags
            //mCompleteTagWithParents.mParentTags.emplace_back(ParentPtr);
        }
    }
}