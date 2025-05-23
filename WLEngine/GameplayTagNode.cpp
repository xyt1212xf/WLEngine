#include "GameplayTagNode.h"
namespace WL
{
    SGameplayTagNode::SGameplayTagNode(const std::string& InTag, const std::string& InFullTag, SGameplayTagNode* ParentPtr)
    {
        mTagName = InTag;
        mFullTagName = InFullTag;
        mParentNodePtr = ParentPtr;
        mCompleteTagWithParents.mGameplayTags.emplace_back(SGameplayTag(InFullTag));
        if (nullptr != ParentPtr && ParentPtr->mTagName.length() > 1)
        {
            //ParentPtr->mCompleteTagWithParents.mGameplayTags
            //mCompleteTagWithParents.mParentTags.emplace_back(ParentPtr);
        }
    }
}