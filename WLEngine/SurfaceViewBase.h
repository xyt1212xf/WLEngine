#pragma once
#include "Resource.h"
#include "Texture.h"
#include "MathLib.h"

namespace WL
{
	class CSurfaceViewBase : public CResource
	{
	public:
		CSurfaceViewBase();
		virtual ~CSurfaceViewBase();
		virtual CTexture* getTexturePtr();
		virtual bool saveToFile(const std::string szFile);
		virtual bool createRenderSurface(int nWidth, int nHeight, UINT32 nTextureFormat, UINT32 depthFormat, UINT bindInfo, UINT32 miscFlags);
		virtual bool createBuffer(void* pData,
			UINT32 nByteWidth, UINT32 nStructStride, UINT32 nUsage, UINT32 nCpuAccessFlag, UINT32 nBindFlag, UINT32 nMiscFlag);

		virtual bool createUAVBuffer(void* pData,
			UINT32 nByteWidth,
			UINT32 nStructStride,
			UINT32 nUsage = USAGE_DEFAULT,
			UINT32 nCpuAccessFlag = 0,
			UINT32 nBindFlag = BIND_UNORDERED_ACCESS | BIND_SHADER_RESOURCE,
			UINT32 nMiscFlag = RESOURCE_MISC_BUFFER_STRUCTURED) ;

		virtual void setRenderSurface(DeviceContext* pContext);
		virtual void clearRenderSurface(DeviceContext* pContext);

		void buildOrthoMT(float fNear, float fFar);
		void buildProjectMT(float fNear, float fFar, float fFov = (float)PI / 4.0f);
		const Matrix44& getOrthoMT()const;
		const Matrix44& getProjectMT()const;

		const Matrix44& getTransposeOrthoMT()const;
		const Matrix44& getTransposeProjectMT()const;
		
		void setSampler(SamplerState* pSampler);
		void applyTexture(DeviceContext* pContext, int nSlot);

	protected:
		CTexture* mpTexture = nullptr;
		INT32 mWidth = 0;
		INT32 mHeight = 0;
		float mFar = 1.0f;
		float mNear = 1.0f;
		Matrix44 mProjectMatrix;
		Matrix44 mOrthoMatrix;

		Matrix44 mTransposeProjectMatrix;
		Matrix44 mTransposeOrthoMatrix;
	};
}