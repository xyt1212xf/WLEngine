#pragma once
#include <type_traits>
namespace WL
{
	template <typename T>
	FORCEINLINE constexpr std::remove_reference_t<T>&& MoveTempIfPossible(T&& Obj) noexcept
	{
		using CastType = std::remove_reference_t<T>;
		return (CastType&&)Obj;
	}
}