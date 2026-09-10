#pragma once
#include "Vector.h"
#include "Shader.h"
namespace WL
{
	class CModelInstance;
	class CBoundingSphere 
	{
	public:
		CBoundingSphere();
		virtual ~CBoundingSphere();

	public:
		CModelInstance* mpModelInstance = nullptr;
	};
}
