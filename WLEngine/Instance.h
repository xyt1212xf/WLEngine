#pragma once
#include "Resource.h"
namespace WL
{
	class CInstance : public CResource
	{
	public:
		CInstance();
		virtual ~CInstance();
		virtual CInstance* clone();
	};
}