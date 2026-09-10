#pragma once
#include "Resource.h"
#include "GPUBuffer.h"
#ifdef D3D11
#include "DX11Mesh.h"
#endif

namespace WL
{
#ifdef D3D11
	class CMesh : public CDX11Mesh 
#endif
	{
	public: 
		CMesh();
		virtual ~CMesh();


	private:

	};
	
}