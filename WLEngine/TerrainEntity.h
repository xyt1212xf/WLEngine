#pragma once
#include "Entity.h"
#include "Transform.h"
#include "InputMgr.h"
namespace WL
{
	class CTerrainEntity : public CEntity
						 , public CController
						 , public CTranformComponet

	{
	public:
		CTerrainEntity();
		virtual ~CTerrainEntity();

	private:
	};
}