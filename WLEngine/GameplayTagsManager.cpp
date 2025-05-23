#include "GameplayTagsManager.h"
#include "GameplayTagNode.h"
#include "GameplayTagContainer.h"
#include "Foundation.h"
#include "jsonHeader.h"
#include "WLEngine.h"

namespace WL 
{
	const SGameplayTag* SGameplayTagRedirect::redirectTag(const std::string& InTagName) const
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
		for (auto& item : CGameplayTagsManager::getSinglePtr()->mGameplayTagNodeMap)
		{
			WL_DELETE(item.second, GAS);
		}
		CGameplayTagsManager::getSinglePtr()->mGameplayTagNodeMap.clear();
		WL_DELETE(CGameplayTagsManager::getSinglePtr()->mGameplayRootTagPtr, GAS);
		CGameplayTagsManager::destory();
	}

	SGameplayTag CGameplayTagsManager::requestGameplayTag(const std::string& TagName, bool ErrorIfNotFound) const
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

		const SGameplayTag Possible(TagName);
		if (auto iter = mGameplayTagNodeMap.find(Possible); iter != mGameplayTagNodeMap.end())
		{
			return Possible;
		}
		return SGameplayTag();
	}
	
	void CGameplayTagsManager::requestGameplayTagChildren(const std::string& TagName, std::vector<SGameplayTag>& ChildTags)
	{
		if (nullptr != mGameplayRootTagPtr)
		{

		}
	}

	void CGameplayTagsManager::getAllGameplayTags(std::vector<SGameplayTag>& ChildTags)
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
									SGameplayTagNode* curNodePtr = mGameplayRootTagPtr;
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
			mTagRedirect.mTags["weapon"] = SGameplayTag("weapon");
			mTagRedirect.mTags["attack"] = SGameplayTag("attack");
			mTagRedirect.mTags["defense"] = SGameplayTag("defense");
			SGameplayTag("InputUserSettings.Profiles.Default").matchesTag(SGameplayTag("InputUserSettings"));
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
			mGameplayRootTagPtr = WL_NEW(SGameplayTagNode, GAS);
		}
	}

	INT32 CGameplayTagsManager::insertTagIntoNodeArray(const std::string& InTag, const std::string& InFullTag, SGameplayTagNode* ParentNodePtr, std::vector<SGameplayTagNode*>& NodeArray)
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
			SGameplayTagNode* CurrNode = NodeArray[LowerBoundIndex];
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
	
			auto TagNode = WL_NEW(SGameplayTagNode, GAS)(InTag, InFullTag, ParentNodePtr);
			NodeArray.insert(NodeArray.begin() + WhereToInsert, TagNode);
			SGameplayTag gameplayTag = TagNode->getCompleteTag();
			assert(gameplayTag.getTagName() == InFullTag);
			mGameplayTagNodeMap[gameplayTag] = TagNode;
		}

		return FoundNodeIdx;
	}
	
}
