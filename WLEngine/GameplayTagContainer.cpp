#include "GameplayTagContainer.h"
#include "GameplayTagsManager.h"
#include "GameplayTagNode.h"
#include "WLEngine.h"

namespace WL
{
	DefineScriptClass(SGameplayTag)
	const  SGameplayTag SGameplayTag::EmptyTag;
	SGameplayTag::SGameplayTag()
	{
	}

	SGameplayTag::SGameplayTag(const std::string& szName)
	: mTagName(szName)
	{
	}

	SGameplayTag::~SGameplayTag()
	{

	}

	SGameplayTag SGameplayTag::requestGameplayTag(const std::string& TagName, bool ErrorIfNotFound)
	{
		auto pMgr = CGameplayTagsManager::getSinglePtr();
		return SGameplayTag();
	}

	bool SGameplayTag::matchesTag(const SGameplayTag& TagToCheck) const
	{
		bool bResult = false;
		if (auto pMgr = CGameplayTagsManager::getSinglePtr())
		{
			auto pNode = pMgr->findTagNode(*this);
			while (pNode)
			{
				if (pNode->mFullTagName == TagToCheck.getTagName())
				{
					bResult = true;
					break;
				}
				else
				{
					pNode = (pNode->mParentNodePtr);
				}
			}
		}
		return bResult;
	}

	void SGameplayTag::registerClass()
	{
		CRegisterClass<SGameplayTag>::registerClass(GEngine->getLuaState());
	}

	void SGameplayTag::registerMember()
	{
		auto pLua = GEngine->getLuaState();
		CRegisterFun<std::string>::registerClassMemberFun<SGameplayTag, std::string(SGameplayTag::*)()const, &SGameplayTag::getTagName>(pLua, "getName");
		CRegisterFun<void>::registerClassMemberFun<SGameplayTag, void(SGameplayTag::*)(const std::string&), &SGameplayTag::setTagName, const std::string&>(pLua, "setName");
	}

	/// <summary>
	/// GameplayTagContainer  
	/// </summary>
	SGameplayTagContainer& SGameplayTagContainer::operator=(const SGameplayTagContainer& Other)
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

	SGameplayTagContainer& SGameplayTagContainer::operator=(SGameplayTagContainer&& Other)
	{
		mGameplayTags = std::move(Other.mGameplayTags);
		mParentTags = std::move(Other.mParentTags);
		return *this;
	}

	void SGameplayTagContainer::addTag(const SGameplayTag& TagToAdd)
	{
		if (auto iter = std::find(mGameplayTags.begin(), mGameplayTags.end(), TagToAdd); iter == mGameplayTags.end())
		{
			mGameplayTags.emplace_back(TagToAdd);	
		}
	}

	void SGameplayTagContainer::removeTag(const SGameplayTag& TagToRemove)
	{
		if (auto iter = std::find(mGameplayTags.begin(), mGameplayTags.end(), TagToRemove); iter != mGameplayTags.end())
		{			
			mGameplayTags.erase(iter);
		}
	}


	SGameplayTag SGameplayTagContainer::first() const
	{
		return mGameplayTags.size() > 0 ? mGameplayTags.front() : SGameplayTag();
	}

	SGameplayTag SGameplayTagContainer::last() const
	{
		return mGameplayTags.size() > 0 ? mGameplayTags.back() : SGameplayTag();
	}

}
