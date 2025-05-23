#pragma once
#include "Common.h"
#include "ScriptEntity.h"

namespace WL
{
	struct SGameplayTag : public CScriptEntity
	{
		/** An empty Gameplay Tag */
		static const SGameplayTag EmptyTag;

		SGameplayTag();
		virtual ~SGameplayTag();

		explicit SGameplayTag(const std::string& szName);

		static SGameplayTag requestGameplayTag(const std::string& TagName, bool ErrorIfNotFound = true);

		std::string getTagName() const
		{
			return mTagName;
		}
		
		void setTagName(const std::string& szName)
		{
			mTagName = szName;
			if (false)
			{
				matchesTag(*this);
			}
		}

		bool isValid() const
		{
			return mTagName.length() > 0 ? true : false;
		}

		bool matchesTag(const SGameplayTag& TagToCheck) const;

		INLINE bool matchesTagExact(const SGameplayTag& TagToCheck) const
		{
			if (!TagToCheck.isValid())
			{
				return false;
			}
			// Only check check explicit tag list
			return mTagName == TagToCheck.mTagName;
		}

		INLINE bool operator==(SGameplayTag const& Other) const
		{
			return mTagName == Other.mTagName;
		}

		INLINE bool operator!=(SGameplayTag const& Other) const
		{
			return mTagName != Other.mTagName;
		}

		bool operator < (const SGameplayTag& other) const
		{
			return mTagName < other.mTagName;
		}

		std::string mTagName = "";

		DeclareScriptClass(SGameplayTag);
	};

	struct SGameplayTagContainer
	{
		SGameplayTagContainer() = default;

		SGameplayTagContainer(SGameplayTagContainer&& Other) noexcept
		{
			mGameplayTags = std::move(Other.mGameplayTags);
			mParentTags = std::move(Other.mParentTags);
		}

		SGameplayTagContainer(const SGameplayTagContainer& Other)
		{
			*this = Other;
		}

		explicit SGameplayTagContainer(const SGameplayTag& Tag) 
		{

		}
		SGameplayTagContainer& operator=(const SGameplayTagContainer& Other);
		SGameplayTagContainer& operator=(SGameplayTagContainer&& Other);

		/** Returns the number of explicitly added tags */
		INLINE INT32 num() const
		{
			return static_cast<INT32>(mGameplayTags.size());
		}

		void addTag(const SGameplayTag& TagToAdd);
		
		void removeTag(const SGameplayTag& TagToRemove);

		SGameplayTag first() const;
	
		SGameplayTag last() const;
	
		/** Returns the number of explicitly added tags */
		INLINE size_t count() const
		{
			return mGameplayTags.size();
		}

		/** Returns whether the container has any valid tags */
		INLINE bool isValid() const
		{
			return mGameplayTags.size() > 0;
		}

		/** Returns true if container is empty */
		INLINE bool isEmpty() const
		{
			return mGameplayTags.size() == 0;
		}

		INLINE bool hasTag(const SGameplayTag& TagToCheck) const
		{
			if (TagToCheck.isValid())
			{
				auto iter = std::find(mGameplayTags.begin(), mGameplayTags.end(), TagToCheck);
				if (iter != mGameplayTags.end())
				{
					return true;
				}
			}
			return false;
		}
		std::vector<SGameplayTag> mGameplayTags;
		std::vector<SGameplayTag> mParentTags;
	};
}