#include "GameplayTagContainer.h"
#include "GameplayTagsManager.h"
#include "WLEngine.h"

namespace WL
{
	DefineScriptClass(GameplayTag)
	const  GameplayTag GameplayTag::EmptyTag;
	GameplayTag::GameplayTag()
	{
	}

	GameplayTag::GameplayTag(const std::string& szName)
	: mTagName(szName)
	{
	}

	GameplayTag::~GameplayTag()
	{

	}

	GameplayTag GameplayTag::requestGameplayTag(const std::string& TagName, bool ErrorIfNotFound)
	{
		auto pMgr = CGameplayTagsManager::getSinglePtr();
		return GameplayTag();
	}

	bool GameplayTag::matchesTag(const GameplayTag& TagToCheck) const
	{
		bool bResult = false;
		auto pMgr = CGameplayTagsManager::getSinglePtr();
		if (nullptr != pMgr)
		{
			auto pNode = pMgr->FindTagNode(mTagName);
			if (pNode)
			{

			}
			pNode = nullptr;
		}
		return bResult;
	}

	void GameplayTag::registerClass()
	{
		CRegisterClass<GameplayTag>::registerClass(GEngine->getLuaState());
	}

	void GameplayTag::registerMember()
	{
		auto pLua = GEngine->getLuaState();
		CRegisterFun<std::string>::registerClassMemberFun<GameplayTag, std::string(GameplayTag::*)()const, &GameplayTag::getTagName>(pLua, "getName");
		CRegisterFun<void>::registerClassMemberFun<GameplayTag, void(GameplayTag::*)(const std::string&), &GameplayTag::setTagName, const std::string&>(pLua, "setName");
	}

	/// <summary>
	/// GameplayTagContainer  
	/// </summary>
	GameplayTagContainer& GameplayTagContainer::operator=(const GameplayTagContainer& Other)
	{
		// Guard against self-assignment
		if (this == &Other)
		{
			return *this;
		}
		mGameplayTags.insert(mGameplayTags.end(), Other.mGameplayTags.begin(), Other.mGameplayTags.end());
		mParentTags.insert(mParentTags.end(), Other.mParentTags.begin(), Other.mParentTags.end());
		return *this;
	}

	GameplayTagContainer& GameplayTagContainer::operator=(GameplayTagContainer&& Other)
	{
		mGameplayTags = std::move(Other.mGameplayTags);
		mParentTags = std::move(Other.mParentTags);
		return *this;
	}

	void GameplayTagContainer::addTag(const GameplayTag& TagToAdd)
	{
		if (auto iter = std::find(mGameplayTags.begin(), mGameplayTags.end(), TagToAdd); iter == mGameplayTags.end())
		{
			mGameplayTags.emplace_back(TagToAdd);	
		}
	}

	void GameplayTagContainer::removeTag(const GameplayTag& TagToRemove)
	{
		if (auto iter = std::find(mGameplayTags.begin(), mGameplayTags.end(), TagToRemove); iter != mGameplayTags.end())
		{			
			mGameplayTags.erase(iter);
		}
	}


	GameplayTag GameplayTagContainer::first() const
	{
		return mGameplayTags.size() > 0 ? mGameplayTags.front() : GameplayTag();
	}

	GameplayTag GameplayTagContainer::last() const
	{
		return mGameplayTags.size() > 0 ? mGameplayTags.back() : GameplayTag();
	}

}
