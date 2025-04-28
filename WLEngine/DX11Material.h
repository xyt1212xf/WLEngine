#pragma once

#include "MaterialBase.h"
namespace WL
{
	class CDX11Material : public CMaterialBase
	{
	public:
		CDX11Material();
		virtual ~CDX11Material();
		bool initialiseSampler(SamplerDesc* pSampler);
	};
}