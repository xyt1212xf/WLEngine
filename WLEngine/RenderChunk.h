#pragma once
#include "RenderStateHeader.h"
#include "GpuBuffer.h"
#include "MaterialInstance.h"
#include "ShaderInstance.h"
#include "MeshInstance.h"
#include "Matrix.h"
#include "Vector.h"

namespace WL
{
	struct SGeometry
	{
		SMatrix44 pMatrix[8];
		UINT32 vertexSize = 0;
		UINT32 vertexOffset = 0;
		UINT32 indexType = 0;
		UINT32 indexCount = 0;
		UINT32 startIndex = 0;
		UINT32 baseVertexLocation = 0;
		CGpuBuffer* pVB = nullptr;
		CGpuBuffer* pIB = nullptr;
		CMaterialInstance* pMaterialInstance = nullptr;
		std::map<TextureUseType, CTexture*>* pTextures = nullptr;
	};

	enum enumMatrixType
	{
		WORLD_MT_0,
		VIEW_MT,
		PROJECT_MT,
		VIEW_PROJECT_MT,
		WORLD_MT_1,
		WORLD_MT_2,
		WORLD_MT_3,
	};

	struct SRenderUnit
	{
		//data
		CMaterialInstance*	pMaterialInstance = nullptr;
		std::map<UINT16, std::list<SGeometry>>	geometry;
		//function
		void reset();
	};

	struct SRenderUnitGrounp
	{
		SRenderUnit* getRenderUnit();
		void clearRenderUnit();
		~SRenderUnitGrounp();
		std::list<SRenderUnit*>	mpRenderUnits;
		std::list<SRenderUnit*>	mFreeRenderUnits;
	};
}