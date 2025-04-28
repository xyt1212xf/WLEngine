#pragma once
#include <string>
#include "Mesh.h"
#include "Font.h"
#include "UIWnd.h"
#include "Model.h"
#include "Shader.h"
#include "Material.h"
#include "Texture.h"
#include "SurfaceView.h"
#include "MemoryMgr.h"

namespace WL
{
	class CResourceFactory 
	{
	public:
		CResourceFactory();
		virtual ~CResourceFactory();
		CResource* createResource(ResourceType type);
	};
}