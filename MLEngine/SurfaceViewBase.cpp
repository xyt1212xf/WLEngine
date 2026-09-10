#include "SurfaceViewBase.h"
namespace WL
{

	CSurfaceViewBase::CSurfaceViewBase()
	{

	}

	CSurfaceViewBase::~CSurfaceViewBase()
	{
		WL_DECREASE(mpTexture);
	}

	CTexture* CSurfaceViewBase::getTexturePtr()
	{
		return mpTexture;
	}

	bool CSurfaceViewBase::saveToFile(const std::string szFile)
	{
		return false;
	}

	bool CSurfaceViewBase::createRenderSurface(int nWidth, int nHeight, UINT32 nTextureFormat, UINT32 depthFormat, UINT bindInfo, UINT32 miscFlags)
	{
		return false;
	}

	bool CSurfaceViewBase::createBuffer(void* pData,
		UINT32 nByteWidth, UINT32 nStructStride, UINT32 nUsage, UINT32 nCpuAccessFlag, UINT32 nBindFlag, UINT32 nMiscFlag)
	{
		return false;
	}

	bool CSurfaceViewBase::createUAVBuffer(void* pData, UINT32 nByteWidth, UINT32 nStructStride, UINT32 nUsage /*= USAGE_DEFAULT*/, UINT32 nCpuAccessFlag /*= 0*/, UINT32 nBindFlag /*= BIND_UNORDERED_ACCESS | BIND_SHADER_RESOURCE*/, UINT32 nMiscFlag /*= RESOURCE_MISC_BUFFER_STRUCTURED*/)
	{
		return false;
	}

	void CSurfaceViewBase::setRenderSurface(DeviceContext* pContext)
	{	

	}

	void CSurfaceViewBase::clearRenderSurface(DeviceContext* pContext)
	{

	}

	void CSurfaceViewBase::buildOrthoMT(float fNear, float fFar)
	{
		// Create an orthographic projection matrix for 2D rendering.
		Math::buildOrthographicMatrixLH(&mOrthoMatrix, (float)mWidth, (float)mHeight, fNear, fFar);
		Math::matrixTranspose(&mTransposeOrthoMatrix, &mOrthoMatrix);
	}

	void CSurfaceViewBase::buildProjectMT(float fNear, float fFar, float fFov)
	{
		// Setup the projection matrix.
		Math::buildMatrixPerspectiveFovLH(&mProjectMatrix, fFov, (float)mWidth / (float)mHeight, fNear, fFar);
		Math::matrixTranspose(&mTransposeProjectMatrix, &mProjectMatrix);

	}

	const SMatrix44& CSurfaceViewBase::getOrthoMT() const
	{
		return mOrthoMatrix;
	}

	const SMatrix44& CSurfaceViewBase::getProjectMT() const
	{
		return mProjectMatrix;
	}


	const SMatrix44& CSurfaceViewBase::getTransposeOrthoMT() const
	{
		return mTransposeOrthoMatrix;
	}

	const SMatrix44& CSurfaceViewBase::getTransposeProjectMT() const
	{
		return mTransposeProjectMatrix;
	}

	void CSurfaceViewBase::setSampler(SamplerState* pSampler)
	{
		if (nullptr != pSampler )
		{
			mpTexture->setSampleState(pSampler);
		}
	}

	void CSurfaceViewBase::applyTexture(DeviceContext* pContext, int nSlot)
	{
		mpTexture->use(pContext, nSlot);
	}

}