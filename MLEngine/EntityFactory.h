#pragma once
#include "Entity.h"
#include "CameraEntity.h"
#include "ActorEntity.h"
#include "LightEntity.h"
#include "SkyEntity.h"
#include "SkyBoxEntity.h"
#include "TerrainEntity.h"
#include "OceanEntity.h"
#include "GuiEntity.h"

namespace WL
{
	class CEntityFactory
	{
	public:
		CEntityFactory() = default;		
		CEntity* createEntity(EntityType type);
		~CEntityFactory();
	};
}