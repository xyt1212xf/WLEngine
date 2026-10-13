#pragma once
#include "Object.h"
namespace ML
{
	class MLENGINE_API CStreamableRenderAsset : public CObject
	{
	public:
		CStreamableRenderAsset() = default;
		virtual ~CStreamableRenderAsset();
	};
}
