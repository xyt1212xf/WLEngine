#pragma once
#include "Common.h"
namespace WL
{
	class CNonCopyable
	{
	protected:
		CNonCopyable() = default;
		~CNonCopyable() = default;

	private:
		CNonCopyable(const CNonCopyable&) = delete;
		const CNonCopyable& operator=(const CNonCopyable&) = delete;
	};
}