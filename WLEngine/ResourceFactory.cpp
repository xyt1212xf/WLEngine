#include "ResourceFactory.h"
#include "WLEngine.h"
namespace WL
{

	CResourceFactory::CResourceFactory()
	{

	}

	CResourceFactory::~CResourceFactory()
	{

	}

	CResource* CResourceFactory::createResource(ResourceType type)
	{
		CResource* pResource = nullptr;
		switch (type)
		{
		case SurfaceView:
		{
			pResource = WL_NEW(CSurfaceView, SurfaceView);
			pResource->mTypeName = "RenderSurface";
			break;
		}
		case Mesh:
		{
			pResource = WL_NEW(CMesh, Mesh);
			pResource->mTypeName = "Mesh";
			break;
		}
		case Texture:
		{
			pResource = WL_NEW(CTexture, Texture);
			pResource->mTypeName = "Texture";
			break;
		}
		case Shader:
		{
			pResource = WL_NEW(CShader, Shader);
			pResource->mTypeName = "Shader";
			break;
		}
		case Material:
		{
			pResource = WL_NEW(CMaterial, Material);
			pResource->mTypeName = "Material";
			break;
		}
		case Wnd:
		{
			pResource = GEngine->createUI(ui_wnd);
			break;
		}
		case Font:
		{
			pResource = WL_NEW(CFont, Font);
			pResource->mTypeName = "Font";
			break;
		}
		case Model:
		{
			pResource = WL_NEW(CModel, Model);
			pResource->mTypeName = "Model";
			break;
		}
		}
		pResource->mType = type;
		return pResource;
	}

}