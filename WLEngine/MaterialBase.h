#pragma once
#include "Resource.h"
#include "Shader.h"
#include "Texture.h"

namespace WL
{
	class CMaterialBase : public CResource
	{
	public:
		CMaterialBase();
		virtual ~CMaterialBase();
		SamplerState* getSampler(int index = 0)const;
		bool setSampler(DeviceContext* pContext, int nStart, int nIndex );
		//std::vector<CTexture*>& getTextures();
		bool isShadow();
		bool isLight();

		void addVSValue(const std::string& name, INT32 size, INT32 nRegister, INT32 nOffset);
		void addPSValue(const std::string& name, INT32 size, INT32 nRegister, INT32 nOffset);
		void addDSValue(const std::string& name, INT32 size, INT32 nRegister, INT32 nOffset);
		void addHSValue(const std::string& name, INT32 size, INT32 nRegister, INT32 nOffset);
		void addGSValue(const std::string& name, INT32 size, INT32 nRegister, INT32 nOffset);
		void addStructValue(const std::string& name, INT32 size, INT32 nStructSize, INT32 nRegister, INT32 nOffset);

		void addCSValue(const std::string& name, INT32 size, INT32 nRegister, INT32 nOffset, 
			int Usage, UINT32 BindFlags, UINT32 CPUAccessFlags, SUBRESOURCE_DATA* pSubDdata = nullptr);

		void addTextureBufferValue(const std::string& name, int nRegister, const ImgFileInfo& fileInfo, INT32 nShaderFlags,
			int Usage = USAGE_DYNAMIC,
			UINT32 BindFlags = BIND_SHADER_RESOURCE,
			UINT32 CPUAccessFlags = CPU_ACCESS_WRITE);

	protected:
		INT		mDepthCompare = 1;		//默认深度比较开启
		INT		mDepth = 1;				//默认深度写入开启
		INT		mDepthWrite = 1;
		INT     mStencil = 0;
		INT		mCullMode = 1;			//;
		INT		mFillMode = 2;			//;
		INT		mTransparent = 0;
		INT		mShadow = 0;
		INT	    mTopology = PRIMITIVE_TOPOLOGY_TRIANGLELIST;
		INT		mEnableLight = 0;
		std::list<CShader*>			mpShaders;
		std::vector<SamplerState*>	mSampleState;

		Vec4F	mAmbientColor = {1,1,1,1};
		Vec4F	mDiffuseColor = { 1,1,1,1 };
		Vec4F	mSpecularColor = { 1,1,1,1 }; // w = 镜面反射强度
	};
}