#pragma once
#include "Instance.h"
#include "Shader.h"
#include "TextureArray.h"
#ifdef D3D11
#include "DX11ShaderInstance.h"
#endif

namespace WL
{
#ifdef D3D11
	class CShaderInstance : public CDX11ShaderInstance 
#endif
	{
	public:
		CShaderInstance(CShader* pShader);
		virtual ~CShaderInstance();
		void bindTextureArray(const CTextureArray& textureArray);
		void bindTextureArray(const CTextureArray* pTextureArray);
		void unbindTextureArray();

		void addTexture(int nIndex, CTexture* pTexture, INT32 nFlag = ShaderBufferType::PS);
		void bindTexture(int nIndex, CTexture* pTexture, INT32 nFlag = ShaderBufferType::PS);

		void removeTexture(int nIndex);
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

	private:
		void bindShader(CShader* pShader);
	};
}