#pragma once
#include <type_traits>
namespace ML
{
	template <typename T>
	FORCEINLINE constexpr std::remove_reference_t<T>&& MoveTempIfPossible(T&& Obj) noexcept
	{
		using CastType = std::remove_reference_t<T>;
		return (CastType&&)Obj;
	}

#ifdef _DEBUG
	template<typename T>
	constexpr bool IsValidAddress(const T* Addr, const T* Data, size_t Max)
	{
		bool bResult = Addr >= Data && Addr < (Data + Max);
		if (bResult)
		{
			std::stringstream ss;
			ss << "Attempting to use a container element (" << (void*)Addr
				<< ") which already comes from the container being modified ("
				<< (void*)Data << ", ArrayMax: " << Max
				<< ", ArrayNum: " << Max
				<< ", SizeofElement: " << sizeof(T) << ")!";

			throw std::runtime_error(ss.str());
		}
		return bResult;
	}
#else 
	template<typename T>
	constexpr bool IsValidAddress(const T* Addr, const T* Data, size_t Max){return true;}
#endif
}