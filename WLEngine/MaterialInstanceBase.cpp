#include "MaterialInstanceBase.h"
#include "WLEngine.h"

namespace WL
{

	CMaterialInstanceBase::CMaterialInstanceBase()
	{

	}

	CMaterialInstanceBase::~CMaterialInstanceBase()
	{

	}

	void CMaterialInstanceBase::updateVSConstBuffer(int nRegistIndex, void* pData)
	{
		for( auto& item : mArrayShaderIns)
		{
			item->updateVSConstBuffer(nRegistIndex, pData);
		}
	}

	void CMaterialInstanceBase::updateVSConstBuffer(const std::string& szName, void* pData)
	{
		for (auto& item : mArrayShaderIns)
		{
			item->updateVSConstBuffer(szName, pData);
		}
	}

	void CMaterialInstanceBase::updateTextureConstBuffer(int nRegistIndex, void* pData)
	{
		for (auto& item : mArrayShaderIns)
		{
			item->updateTextureConstBuffer(nRegistIndex, pData);
		}
	}

	void CMaterialInstanceBase::updateTextureConstBuffer(const std::string& name, void* pData)
	{
		for (auto& item : mArrayShaderIns)
		{
			item->updateTextureConstBuffer(name, pData);
		}
	}

	void CMaterialInstanceBase::updateTextureBuffer(int nRegistIndex, void* pData)
	{
		for (auto& item : mArrayShaderIns)
		{
			item->updateTextureBuffer(nRegistIndex, pData);
		}
	}

	void CMaterialInstanceBase::updateTextureBuffer(const std::string& name, void* pData)
	{
		for (auto& item : mArrayShaderIns)
		{
			item->updateTextureBuffer(name, pData);
		}
	}

	void CMaterialInstanceBase::updateTypeBuffer(int nRegistIndex, void* pData)
	{
		for (auto& item : mArrayShaderIns)
		{
			item->updateTypeBuffer(nRegistIndex, pData);
		}
	}

	void CMaterialInstanceBase::updateTypeBuffer(const std::string& name, void* pData)
	{
		for (auto& item : mArrayShaderIns)
		{
			item->updateTypeBuffer(name, pData);
		}
	}

	void CMaterialInstanceBase::updateStructedBuffer(int nRegistIndex, void* pData)
	{
		for (auto& item : mArrayShaderIns)
		{
			item->updateStructedBuffer(nRegistIndex, pData);
		}
	}

	void CMaterialInstanceBase::updateStructedBuffer(const std::string& name, void* pData)
	{
		for (auto& item : mArrayShaderIns)
		{
			item->updateStructedBuffer(name, pData);
		}
	}

	void CMaterialInstanceBase::updateHSConstBuffer(int nRegistIndex, void* pData)
	{
		for (auto& item : mArrayShaderIns)
		{
			item->updateHSConstBuffer(nRegistIndex, pData);
		}
	}

	void CMaterialInstanceBase::updateHSConstBuffer(const std::string& name, void* pData)
	{
		for (auto& item : mArrayShaderIns)
		{
			item->updateHSConstBuffer(name, pData);
		}
	}

	void CMaterialInstanceBase::updateDSConstBuffer(int nRegistIndex, void* pData)
	{
		for (auto& item : mArrayShaderIns)
		{
			item->updateDSConstBuffer(nRegistIndex, pData);
		}
	}

	void CMaterialInstanceBase::updateDSConstBuffer(const std::string& name, void* pData)
	{
		for (auto& item : mArrayShaderIns)
		{
			item->updateDSConstBuffer(name, pData);
		}
	}

	void CMaterialInstanceBase::updatePSConstBuffer(int nRegistIndex, void* pData)
	{
		for (auto& item : mArrayShaderIns)
		{
			item->updatePSConstBuffer(nRegistIndex, pData);
		}
	}

	void CMaterialInstanceBase::updatePSConstBuffer(const std::string& name, void* pData)
	{
		for (auto& item : mArrayShaderIns)
		{
			item->updatePSConstBuffer(name, pData);
		}
	}

	void CMaterialInstanceBase::updateGSConstBuffer(int nRegistIndex, void* pData)
	{
		for (auto& item : mArrayShaderIns)
		{
			item->updateGSConstBuffer(nRegistIndex, pData);
		}
	}

	void CMaterialInstanceBase::updateGSConstBuffer(const std::string& name, void* pData)
	{
		for (auto& item : mArrayShaderIns)
		{
			item->updateGSConstBuffer(name, pData);
		}
	}

	void CMaterialInstanceBase::updateCSConstBuffer(int nRegistIndex, void* pData)
	{
		for (auto& item : mArrayShaderIns)
		{
			item->updateCSConstBuffer(nRegistIndex, pData);
		}
	}

	void CMaterialInstanceBase::updateCSConstBuffer(const std::string& name, void* pData)
	{
		for (auto& item : mArrayShaderIns)
		{
			item->updateCSConstBuffer(name, pData);
		}
	}

	void CMaterialInstanceBase::setFillCullMode(int nValue)
	{
		mRenderStateDesc.fillMode = nValue;
	}

	void CMaterialInstanceBase::setAlpha(int nValue)
	{
		mRenderStateDesc.transparent = nValue != 0 ? true : false;
	}

	void CMaterialInstanceBase::setDepthStencil(int nValue)
	{
		mRenderStateDesc.stencil = nValue != 0 ? true : false;
	}

	void CMaterialInstanceBase::setDepth(int nValue)
	{
		mRenderStateDesc.depth = nValue != 0 ? true : false;
	}

	void CMaterialInstanceBase::setDepthCompare(int nValue)
	{
		mRenderStateDesc.depthCompare = nValue;
	}

	void CMaterialInstanceBase::setLight(int nValue)
	{
		mRenderStateDesc.enableLight = (nValue == 0 ? false : true);
	}

	bool CMaterialInstanceBase::isAlpha()
	{
		return mRenderStateDesc.transparent;
	}

	bool CMaterialInstanceBase::isDepth()
	{
		return mRenderStateDesc.depth;
	}

	bool CMaterialInstanceBase::isShadow()
	{
		return mRenderStateDesc.shadow;
	}

	bool CMaterialInstanceBase::isLight()
	{
		return mRenderStateDesc.enableLight;
	}

	void CMaterialInstanceBase::setTopology(int nType)
	{
		for( auto item : mArrayShaderIns)
		{
			item->setTopology(nType);
		}
	}

	bool CMaterialInstanceBase::setTexture(const std::string& szTexture, int nType/*= Base*/)
	{
		std::string strExtension = szTexture;
		std::transform(strExtension.begin(), strExtension.end(), strExtension.begin(), ::tolower);
		auto pTexture = dynamic_cast<CTexture*>(GEngine->createResource(szTexture, Texture));
		return (nullptr != pTexture) ? setTexture(pTexture, nType) : false;
	}

	bool CMaterialInstanceBase::setTexture(CTexture* pTexture, int nType/*= Base*/)
	{
		if (nullptr != pTexture)
		{
			for( auto item : mArrayShaderIns )
			{
				item->bindTexture(nType, pTexture);
			}
			return true;
		}
		else
		{
			return false;
		}
	}

	void CMaterialInstanceBase::setAmbientColor(const Vec4F& color)
	{
		mAmbientColor = color;
		for (auto item : mArrayShaderIns)
		{
			item->setPSParamByName("AmbientColor", (void*)&color);
		}
	}

	void CMaterialInstanceBase::setDiffuseColor(const Vec4F& color)
	{
		mDiffuseColor = color;
		for (auto item : mArrayShaderIns)
		{
			item->setPSParamByName("DiffuseColor", (void*)&color);
		}
	}
	
	void CMaterialInstanceBase::setSpecularColor(const Vec4F& color)
	{
		mSpecularColor = color;
		for (auto item : mArrayShaderIns)
		{
			item->setPSParamByName("SpecularColor", (void*)&color);
		}
	}
	
	void CMaterialInstanceBase::setVertexColor(const Vec4F& color)
	{
		mVertexColor = color;
		for (auto item : mArrayShaderIns)
		{
			item->setPSParamByName("VertexColor", (void*)&color);
		}
	}

	const Vec4F& CMaterialInstanceBase::getVertexColor() const
	{
		return mVertexColor;
	}

	void CMaterialInstanceBase::bindRenderState(RenderState* pRenderState)
	{
		mpRenderState = pRenderState;
	}

	RenderState* CMaterialInstanceBase::getRenderState() const
	{
		return mpRenderState;
	}

	CShaderInstance* CMaterialInstanceBase::getShaderInstance(UINT32 nIndex)
	{
		if (nIndex < mArrayShaderIns.size())
		{
			auto iter = mArrayShaderIns.begin();
			std::advance(iter, nIndex);	
			return (*iter);
		}
		else
		{
			return nullptr;
		}
	}

	CShaderInstance* CMaterialInstanceBase::getShaderInstance(const std::string& szName)
	{
		for (auto item : mArrayShaderIns)
		{
			if (item->getName() == szName)
			{
				return item;
			}
		}
		return nullptr;
	}

	const Vec4F& CMaterialInstanceBase::getAmbientColor() const
	{
		return mAmbientColor;
	}

	const Vec4F& CMaterialInstanceBase::getDiffuseColor() const
	{
		return mDiffuseColor;
	}

	const Vec4F& CMaterialInstanceBase::getSpecularColor() const
	{
		return mSpecularColor;
	}

	void CMaterialInstanceBase::setRenderOrder(UINT16 order)
	{
		mRenderOrder = order;
	}

	UINT16 CMaterialInstanceBase::getRenderOrder()
	{
		return mRenderOrder;
	}



}