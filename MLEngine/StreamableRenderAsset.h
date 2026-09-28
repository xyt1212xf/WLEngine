#pragma once
#include "Object.h"
namespace ML
{
	class CStreamableRenderAsset : public CObject
	{
	public:
		CStreamableRenderAsset() = default;
		virtual ~CStreamableRenderAsset();
	};
}