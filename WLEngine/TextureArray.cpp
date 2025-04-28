#include "TextureArray.h"
namespace WL
{
	void CTextureArray::use(DeviceContext* pContext)
	{
		if(mTextureCount > 0 )
		{
			pContext->PSSetSamplers(mSlot, mTextureCount, &mSampleStateVector[0]);
			pContext->PSSetShaderResources(mSlot, mTextureCount, &mShaderRVVector[0]);
		}
	}


	void CTextureArray::setSlot(int nSlot)
	{
		mSlot = nSlot;
	}

	int CTextureArray::getSlot() const
	{
		return mSlot;
	}


	void CTextureArray::operator=(const CTextureArray& textureaArray)
	{
		if (this != &textureaArray)
		{
			Parent::operator=(textureaArray);
			mSlot = textureaArray.mSlot;
		}
	}

}