#pragma once
#include "Common.h"
#include "TSingle.h"
#include "GameplayTagContainer.h"
namespace WL
{
	struct GameplayTagNode;
	
	struct GameplayTagRedirect
	{
		const GameplayTag* redirectTag(const std::string& InTagName) const;
		std::map<std::string, GameplayTag> mTags;
	};


	class CGameplayTagsManager : public  TSingle<CGameplayTagsManager>
	{
	public:
		static void initializeManager();
		static void unInitializeManager();
		GameplayTag requestGameplayTag(const std::string& TagName, bool ErrorIfNotFound) const;
		void requestGameplayTagChildren(const std::string& TagName, std::vector<GameplayTag>& ChildTags);
		void getAllGameplayTags(std::vector<GameplayTag>& ChildTags);
		bool loadGameplayTags(const std::string& szFile);
		
		INLINE GameplayTagNode* findTagNode(const GameplayTag& gameplayTag) const
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
		INLINE GameplayTagNode* findTagNode(const std::string& TagName) const
		{
			GameplayTag PossibleTag(TagName);
			return findTagNode(PossibleTag);
		}

	private:
		void constructGameplayTagTree();
		INT32 insertTagIntoNodeArray(const std::string& InTag, const std::string& InFullTag, GameplayTagNode* ParentNodePtr, std::vector<GameplayTagNode*>& NodeArray);

	private:
		GameplayTagRedirect	mTagRedirect;

		/** Roots of gameplay tag nodes */
		GameplayTagNode* mGameplayRootTagPtr = nullptr;

		/** Map of Tags to Nodes - Internal use only. FGameplayTag is inside node structure, do not use FindKey! */
		std::map<GameplayTag, GameplayTagNode*> mGameplayTagNodeMap;

	};
}