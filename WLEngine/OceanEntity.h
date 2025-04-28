#pragma once
#include "ActorEntity.h"
#include "Transform.h"
#include "OceanSimulator.h"
#include "OceanModelInstance.h"

namespace WL
{
	struct OceanVertex
	{
		float index_x;
		float index_y;
	};

	// Mesh properties:
	class COceanEntity : public CActorEntity
	{
	public:
		COceanEntity();
		virtual ~COceanEntity();
		bool initialise();
		virtual void addModel(CModel* pModel, INT32 nSlot) final;

	private:
		virtual void draw(UINT32 nTime) final;
		virtual void _tick(UINT32 dt) final;
		void _updateDisplacementMap(float time);

	private:
		COceanModelInstance* mpModelInstance = nullptr;
		COceanSimulator mSimulator;
	};
}