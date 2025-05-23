#pragma once
#include "Common.h"
#include "TSingle.h"
#include "GameplayTagContainer.h"
namespace WL
{
	struct SGameplayTagNode;
	
	struct SGameplayTagRedirect
	{
		const SGameplayTag* redirectTag(const std::string& InTagName) const;
		std::map<std::string, SGameplayTag> mTags;
	};


	class CGameplayTagsManager : public  TSingle<CGameplayTagsManager>
	{
	public:
		static void initializeManager();
		static void unInitializeManager();
		SGameplayTag requestGameplayTag(const std::string& TagName, bool ErrorIfNotFound) const;
		void requestGameplayTagChildren(const std::string& TagName, std::vector<SGameplayTag>& ChildTags);
		void getAllGameplayTags(std::vector<SGameplayTag>& ChildTags);
		bool loadGameplayTags(const std::string& szFile);
		
		INLINE SGameplayTagNode* findTagNode(const SGameplayTag& gameplayTag) const
		{
			auto iter = mGameplayTagNodeMap.find(gameplayTag);
			if (iter != mGameplayTagNodeMap.end())
			{
				return (iter->second);
			}
			else
			{
				return nullptr;
			}
		}
		INLINE SGameplayTagNode* findTagNode(const std::string& TagName) const
		{
			SGameplayTag PossibleTag(TagName);
			return findTagNode(PossibleTag);
		}

	private:
		void constructGameplayTagTree();
		INT32 insertTagIntoNodeArray(const std::string& InTag, const std::string& InFullTag, SGameplayTagNode* ParentNodePtr, std::vector<SGameplayTagNode*>& NodeArray);

	private:
		SGameplayTagRedirect	mTagRedirect;

		/** Roots of gameplay tag nodes */
		SGameplayTagNode* mGameplayRootTagPtr = nullptr;

		/** Map of Tags to Nodes - Internal use only. FGameplayTag is inside node structure, do not use FindKey! */
		std::map<SGameplayTag, SGameplayTagNode*> mGameplayTagNodeMap;

	};
}