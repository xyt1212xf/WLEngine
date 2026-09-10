#pragma once
#include "Instance.h"
#include "Material.h"
#include "ShaderInstance.h"
#include "RenderStateHeader.h"
#include "RenderState.h"

namespace WL
{
	class CMaterialInstanceBase : public CInstance
	{
	public:
		CMaterialInstanceBase();
		virtual ~CMaterialInstanceBase();
		void updateVSConstBuffer(int nRegistIndex, void* pData);
		void updateTextureConstBuffer(int nRegistIndex, void* pData);
		void updateTextureBuffer(int nRegistIndex, void* pData);
		void updateTypeBuffer(int nRegistIndex, void* pData);
		void updateStructedBuffer(int nRegistIndex, void* pData);
		void updateHSConstBuffer(int nRegistIndex, void* pData);
		void updateDSConstBuffer(int nRegistIndex, void* pData);
		void updatePSConstBuffer(int nRegistIndex, void* pData);
		void updateGSConstBuffer(int nRegistIndex, void* pData);
		void updateCSConstBuffer(int nRegistIndex, void* pData);

		void updateVSConstBuffer(const std::string& name, void* pData);
		void updateTextureConstBuffer(const std::string& name, void* pData);
		void updateTextureBuffer(const std::string& name, void* pData);
		void updateTypeBuffer(const std::string& name, void* pData);
		void updateStructedBuffer(const std::string& name, void* pData);
		void updateHSConstBuffer(const std::string& name, void* pData);
		void updateDSConstBuffer(const std::string& name, void* pData);
		void updatePSConstBuffer(const std::string& name, void* pData);
		void updateGSConstBuffer(const std::string& name, void* pData);
		void updateCSConstBuffer(const std::string& name, void* pData);

		void setFillCullMode(int nValue);
		void setAlpha(int nValue);
		void setDepthStencil(int nValue);
		void setDepth(int nValue);
		void setDepthCompare(int nValue);
		void setLight(int nValue);
		bool isAlpha();
		bool isDepth();
		bool isShadow();
		bool isLight();
		void setTopology(int nType);
		bool setTexture(const std::string& szTexture, int nType = Base);
		bool setTexture(CTexture* pTexture, int nType = Base);

		void setAmbientColor(const Vec4F& color);
		void setDiffuseColor(const Vec4F& color);
		void setSpecularColor(const Vec4F& color);
		const Vec4F& getAmbientColor() const;
		const Vec4F& getDiffuseColor() const;
		const Vec4F& getSpecularColor() const;
		void setRenderOrder(UINT16 order);
		UINT16 getRenderOrder();
		void setVertexColor(const Vec4F& color);
		const Vec4F& getVertexColor()const;
		void bindRenderState(SRenderState* pRenderState);
		SRenderState* getRenderState()const;
		CShaderInstance* getShaderInstance(UINT32 nIndex);
		CShaderInstance* getShaderInstance(const std::string& szName);

	protected:
		UINT16	mRenderOrder = 0;
		Vec4F	mAmbientColor = {1,1,1,1};
		Vec4F	mDiffuseColor = { 1,1,1,1 };
		Vec4F	mSpecularColor = { 1,1,1,1 }; // w = 镜面反射强度
		Vec4F	mVertexColor = { 0,0,0,1 };
		CMaterial* mpMaterial = nullptr;
		std::list<CShaderInstance*>		mArrayShaderIns;
		SRenderStateDesc					mRenderStateDesc;
		SRenderState*				mpRenderState = nullptr;
	};
}