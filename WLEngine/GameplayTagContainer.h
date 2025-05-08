#pragma once
#include "Common.h"
#include "ScriptEntity.h"

namespace WL
{
	struct GameplayTag : public CScriptEntity
	{
		/** An empty Gameplay Tag */
		static const GameplayTag EmptyTag;

		GameplayTag();
		virtual ~GameplayTag();

		explicit GameplayTag(const std::string& szName);

		static GameplayTag requestGameplayTag(const std::string& TagName, bool ErrorIfNotFound = true);

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

		bool matchesTag(const GameplayTag& TagToCheck) const;

		INLINE bool matchesTagExact(const GameplayTag& TagToCheck) const
		{
			if (!TagToCheck.isValid())
			{
				return false;
			}
			// Only check check explicit tag list
			return mTagName == TagToCheck.mTagName;
		}

		INLINE bool operator==(GameplayTag const& Other) const
		{
			return mTagName == Other.mTagName;
		}

		INLINE bool operator!=(GameplayTag const& Other) const
		{
			return mTagName != Other.mTagName;
		}

		bool operator < (const GameplayTag& other) const
		{
			return mTagName < other.mTagName;
		}

		std::string mTagName = "";

		DeclareScriptClass(GameplayTag);
	};

	struct GameplayTagContainer
	{
		GameplayTagContainer() = default;

		GameplayTagContainer(GameplayTagContainer&& Other) noexcept
		{
			mGameplayTags = std::move(Other.mGameplayTags);
			mParentTags = std::move(Other.mParentTags);
		}

		GameplayTagContainer(const GameplayTagContainer& Other)
		{
			*this = Other;
		}

		explicit GameplayTagContainer(const GameplayTag& Tag) 
		{

		}
		GameplayTagContainer& operator=(const GameplayTagContainer& Other);
		GameplayTagContainer& operator=(GameplayTagContainer&& Other);

		/** Returns the number of explicitly added tags */
		INLINE INT32 num() const
		{
			return static_cast<INT32>(mGameplayTags.size());
		}

		void addTag(const GameplayTag& TagToAdd);
		
		void removeTag(const GameplayTag& TagToRemove);

		GameplayTag first() const;
	
		GameplayTag last() const;
	
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

		INLINE bool hasTag(const GameplayTag& TagToCheck) const
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
		std::vector<GameplayTag> mGameplayTags;
		std::vector<GameplayTag> mParentTags;
	};
}