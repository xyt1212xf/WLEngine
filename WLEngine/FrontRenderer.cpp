#include "FrontRenderer.h"
#include "WLEngine.h"
#include "Scene.h"

namespace WL
{
	CFrontRenderer::CFrontRenderer()
	{
		mOrder = RendererType::FrontRenderer;
		auto pConfig = GEngine->getWindowConfigPtr();

		mpRenderSurface = GEngine->createRenderTarget(RenderTargetType::LightRTT, pConfig->nWidth, pConfig->nHeight, FORMAT_R8G8B8A8_UNORM, 0, 0, RESOURCE_MISC_UNKNOW);
		if (nullptr != mpRenderSurface)
		{
			mpRenderSurface->bindDepthStencilView(GEngine->getDevice()->getDepthStencilView());
			mpRenderSurface->buildOrthoMT(1, 1000);
			mpRenderSurface->addRef();
			mpScreen = GEngine->createBillBoardEntity(R"(resource\Material\drawScreen.ma)", pConfig->nWidth, pConfig->nHeight);
			mpScreen->setPosition(Vec3F(0, 0, 0));
			mpScreen->setViewMT(const_cast<Matrix44*>(&Matrix44::identity()));
			mpScreen->setProjectMT(const_cast<Matrix44*>(&mpRenderSurface->getOrthoMT()));

			mpScreen->setTransposeViewMT(const_cast<Matrix44*>(&Matrix44::identity()));
			mpScreen->setTransposeProjectMT(const_cast<Matrix44*>(&mpRenderSurface->getTransposeOrthoMT()));

		}
	}

	CFrontRenderer::~CFrontRenderer()
	{
		//mpRenderTargetView = nullptr;
		//mpDepthStencilView = nullptr;
		WL_DECREASE(mpScreen);
		WL_DECREASE(mpRenderSurface);
	}

	std::vector<CActorEntity*>& CFrontRenderer::checkEntities(std::vector<CActorEntity*>& pDrawEntitiesArray)
	{
		mDrawEntities.clear();
		for (auto item : pDrawEntitiesArray)
		{
			bool bAlpha = false;
			for (auto model : item->getModels())
			{
				if ((model.second)->isVisual())
				{
					auto& meshsInfo = (model.second)->getAllMeshInfo();
					for (auto& child : meshsInfo)
					{
						if (nullptr != child.pMeshInstance)
						{
							if (child.pMaterialInstance->isAlpha())
							{
								bAlpha = true;
								break;
							}
						}
					}
				}
			}
			if (!bAlpha)
			{
				mDrawEntities.emplace_back(item);
			}
		}
#if 0
		//���һ��ȫ����ȾEbtity
		mDrawEntities.emplace_back(mpScreen);
#endif
		return mDrawEntities;
	}

	void CFrontRenderer::begin()
	{

	}

	void CFrontRenderer::end()
	{

	}

	bool CFrontRenderer::drawBegin(DeviceContext* pDeviceContext, bool bCleanState)
	{
		return Parent::drawBegin(pDeviceContext, bCleanState);
	}

	void CFrontRenderer::drawEnd(DeviceContext* pDeviceContext, CommandList*& pCommandList, int nContext)
	{
		Parent::drawEnd(pDeviceContext, pCommandList, nContext);
	}
}