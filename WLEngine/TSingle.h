#pragma once
#include "Common.h"
namespace WL
{
	template<typename Type>
	class TSingle
	{
	protected:
		TSingle()
		{
			//	Ast(NULL == ms_Singleton);
			mpSingle = static_cast<Type*>(this);
		}

		~TSingle()
		{
			//	Ast(NULL != ms_Singleton);
			mpSingle = nullptr;
		}

	public:
		static Type* createInstance() noexcept
		{
			if (nullptr == mpSingle)
			{
				mpSingle = new Type();
			}
			return mpSingle;
		}

		static void destory()
		{
			SafeDelete(mpSingle);
		}

		static Type* getSinglePtr()
		{
#ifdef DEBUG
			if (nullptr == mpSingle)
			{
				assert(false);
				return nullptr;
			}
#endif // DEBUG
			return mpSingle;
		}
		static Type getSingle()
		{
#ifdef DEBUG
			if (nullptr == mpSingle)
			{
				assert(false);
				return nullptr;
			}
#endif // DEBUG
			return *mpSingle;
		}

	private:
		static Type* mpSingle;
	};

	template <class Type>
	Type* TSingle<Type>::mpSingle = nullptr;
}