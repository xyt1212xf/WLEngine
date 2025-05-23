#pragma once
#ifdef D3D11
#include "DX11OceanSimulator.h"
#endif
#include "FFT_Plan.h"

namespace WL
{
#ifdef D3D11
	class COceanSimulator : public CD3D11OceanSimulator
#endif
	{
		friend class COceanEntity;

	private:
		COceanSimulator();
		virtual ~COceanSimulator();
		// Initialize the vector field.
		virtual bool initialise();
		void setOceanModelInstance(COceanModelInstance* pModelInstance);
		void initNodes();
		int buildNodeList(SQuadNode& quad_node);
		SQuadRenderParam& selectMeshPattern(const SQuadNode& quad_node);

	private:
		bool		mbInitNode = false;
	};
}