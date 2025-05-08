#include "GameplayTagsManager.h"
#include "GameplayTagNode.h"
#include "GameplayTagContainer.h"
#include "Foundation.h"
#include "jsonHeader.h"
#include "WLEngine.h"

namespace WL 
{
	const GameplayTag* GameplayTagRedirect::redirectTag(const std::string& InTagName) const
	{
		auto iter = mTags.find(InTagName);
		return iter != mTags.end() ? &(iter->second) : nullptr;
	}


	//////////////////////////////////////////////////////////////////// 
	void CGameplayTagsManager::initializeManager()
	{
		if (nullptr == CGameplayTagsManager::getSinglePtr())
		{
			CGameplayTagsManager::createInstance();
		}
		CGameplayTagsManager::getSinglePtr()->constructGameplayTagTree();

	}

	void CGameplayTagsManager::unInitializeManager()
	{
		WL_DELETE(CGameplayTagsManager::getSinglePtr()->mGameplayRootTagPtr, GAS);
		CGameplayTagsManager::destory();
	}

	GameplayTag CGameplayTagsManager::requestGameplayTag(const std::string& TagName, bool ErrorIfNotFound) const
	{
		if (nullptr != mGameplayRootTagPtr)
		{
			if (const auto gameplayTagPtr = mTagRedirect.redirectTag(TagName))
			{
				if (auto iter = mGameplayTagNodeMap.find(*gameplayTagPtr); iter != mGameplayTagNodeMap.end())
				{
					return *gameplayTagPtr;
				}
				if (ErrorIfNotFound)
				{
					//std::cout << "Console window is open!" << std::endl;
				}
			}
		}

		const GameplayTag Possible(TagName);
		if (auto iter = mGameplayTagNodeMap.find(Possible); iter != mGameplayTagNodeMap.end())
		{
			return Possible;
		}
		return GameplayTag();
	}
	
	void CGameplayTagsManager::requestGameplayTagChildren(const std::string& TagName, std::vector<GameplayTag>& ChildTags)
	{
		if (nullptr != mGameplayRootTagPtr)
		{

		}
	}

	void CGameplayTagsManager::getAllGameplayTags(std::vector<GameplayTag>& ChildTags)
	{

	}

	bool CGameplayTagsManager::loadGameplayTags(const std::string& szFile)
	{
		bool bResult = true;
		auto strContent = Foundation::readJsonFile(szFile);
		if (strContent.length())
		{
			auto js = json::parse(strContent.c_str());
			auto config = GEngine->getWindowConfigPtr();
			for (auto& item : js.items())
			{
				const auto& k = item.key();
				auto& vavlues = item.value();
				if ("Tags" == k)
				{
					for (const auto& [k, v] : vavlues.items())
					{
						if ("Node" == k)
						{
							for (const auto& obj : v)
							{
								if (obj.contains("TagName"))
								{
									GameplayTagNode* curNodePtr = mGameplayRootTagPtr;
									std::string szContentName = obj["TagName"];							

									auto arrayName = Foundation::splitString<std::string>(szContentName, ".");
									std::string szShortName = "";
									std::string szFullName = "";

									for (size_t i = 0; i < arrayName.size(); ++i)
									{
										szShortName = arrayName[i];
										if (0 == i)
										{
											szFullName = szShortName;
										}
										else
										{
											szFullName.append(".").append(szShortName);
										}
										
										INT32 InsertionIdx = insertTagIntoNodeArray(szShortName, szFullName, curNodePtr, curNodePtr->mChildTags);
										curNodePtr = curNodePtr->mChildTags[InsertionIdx];
									}
								}
							}
						}
					}
				}
			}
			mTagRedirect.mTags["weapon"] = GameplayTag("weapon");
			mTagRedirect.mTags["attack"] = GameplayTag("attack");
			mTagRedirect.mTags["defense"] = GameplayTag("defense");
		}
		else
		{
			bResult = false;
		}

		return bResult;
	}

	void CGameplayTagsManager::constructGameplayTagTree()
	{
		if (nullptr == mGameplayRootTagPtr)
		{
			mGameplayRootTagPtr = WL_NEW(GameplayTagNode, GAS);
		}
	}

	INT32 CGameplayTagsManager::insertTagIntoNodeArray(const std::string& InTag, const std::string& InFullTag, GameplayTagNode* ParentNodePtr, std::vector<GameplayTagNode*>& NodeArray)
	{
		INT32 FoundNodeIdx = -1;
		INT32 WhereToInsert = -1;
		INT32 LowerBoundIndex = 0XFFFF;
		for (size_t i = 0; i < NodeArray.size(); ++i)
		{
			if (NodeArray[i]->mTagName == InTag)
			{
				LowerBoundIndex = static_cast<INT32>(i);
				FoundNodeIdx = LowerBoundIndex;
				break;
			}
		}
		if (LowerBoundIndex < NodeArray.size())
		{
			GameplayTagNode* CurrNode = NodeArray[LowerBoundIndex];
			if (CurrNode->getSimpleTagName() == InTag)
			{

			}
		}
		if (FoundNodeIdx == -1)
		{
			if (WhereToInsert == -1)
			{
				// Insert at end
				WhereToInsert = NodeArray.size();
				FoundNodeIdx = WhereToInsert;
			}
	
			auto TagNode = WL_NEW(GameplayTagNode, GAS)(InTag, InFullTag, ParentNodePtr);
			NodeArray.insert(NodeArray.begin() + WhereToInsert, TagNode);
			GameplayTag gameplayTag = TagNode->getCompleteTag();
			assert(gameplayTag.getTagName() == InFullTag);
			mGameplayTagNodeMap[gameplayTag] = TagNode;
		}

		return FoundNodeIdx;
	}
	
}
