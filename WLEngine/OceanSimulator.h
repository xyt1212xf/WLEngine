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
		int buildNodeList(QuadNode& quad_node);
		QuadRenderParam& selectMeshPattern(const QuadNode& quad_node);

	private:
		bool		mbInitNode = false;
	};
}