#pragma once
#include "ActorEntity.h"
#include "MaterialInstance.h"
namespace WL
{
	class CSkyBoxEntity : public CActorEntity
	{
	public:
		CSkyBoxEntity();
		virtual ~CSkyBoxEntity();
		bool initialise();
	};
}