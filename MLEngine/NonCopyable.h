#pragma once
#include "Common.h"
namespace ML
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