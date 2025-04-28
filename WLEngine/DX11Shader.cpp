#include <iostream>
#include "DX11Shader.h"
#include "WLEngine.h"

namespace WL
{

	CDX11Shader::CDX11Shader()
	{

	}

	CDX11Shader::~CDX11Shader()
	{
		DEV_RELEASE(mpVertexShader);
		DEV_RELEASE(mpHullShader);
		DEV_RELEASE(mpDomainShader);
		DEV_RELEASE(mpPixelShader);
		DEV_RELEASE(mpGeometryShader);
		DEV_RELEASE(mpComputeShader);
		
		for (auto item : mLayoutList)
		{
			DEV_RELEASE(item);
		}
		mLayoutList.clear();

		for (auto item : mTextureBufferMap)
		{
			auto& pair = item.second;
			WL_DECREASE(std::get<2>(pair));
		}
		mTextureBufferMap.clear();

		for (auto item : mVertexConstBufferMap)
		{
			auto& pair = item.second;
			WL_DECREASE(std::get<2>(pair));
		}
		mVertexConstBufferMap.clear();

		for (auto item : mTextureConstBufferMap)
		{
			auto& pair = item.second;
			WL_DECREASE(std::get<2>(pair));
		}
		mTextureConstBufferMap.clear();

		for (auto item : mTypeBufferMap)
		{
			auto& pair = item.second;
			WL_DECREASE(std::get<2>(pair));
		}
		mTypeBufferMap.clear();

		for (auto item : mStructBufferMap)
		{
			auto& pair = item.second;
			WL_DECREASE(std::get<2>(pair));
		}
		mStructBufferMap.clear();

		for (auto item : mHullConstBufferMap)
		{
			auto& pair = item.second;
			WL_DECREASE(std::get<2>(pair));
		}
		mHullConstBufferMap.clear();

		for (auto item : mDomainConstBufferMap)
		{
			auto& pair = item.second;
			WL_DECREASE(std::get<2>(pair));
		}
		mDomainConstBufferMap.clear();

		for (auto item : mPixelConstBufferMap)
		{
			auto& pair = item.second;
			WL_DECREASE(std::get<2>(pair));
		}
		mPixelConstBufferMap.clear();

		for (auto item : mGeometryConstBufferMap)
		{
			auto& pair = item.second;
			WL_DECREASE(std::get<2>(pair));
		}
		mGeometryConstBufferMap.clear();

		for (auto item : mComputeConstBufferMap)
		{
			auto& pair = item.second;
			WL_DECREASE(std::get<2>(pair));
		}
		mComputeConstBufferMap.clear();

		for (auto item : mTextureBufferResourceViewMap)
		{
			DEV_RELEASE(item.second);
		}
		mTextureBufferResourceViewMap.clear();

		for (auto item : mTextureConstResourceViewMap)
		{
			DEV_RELEASE(item.second);
		}
		mTextureConstResourceViewMap.clear();

		for (auto item : mTypeResourceViewMap)
		{
			DEV_RELEASE(item.second);
		}
		mTypeResourceViewMap.clear();

		for (auto item : mStructedResourceViewMap)
		{
			DEV_RELEASE(item.second);
		}
		mStructedResourceViewMap.clear();
	}

	void CDX11Shader::use(DeviceContext* pDeviceContext)
	{
		// Set the vertex and pixel shaders that will be used to render this triangle.
		pDeviceContext->VSSetShader(mpVertexShader, NULL, 0);
		pDeviceContext->HSSetShader(mpHullShader, NULL, 0);
		pDeviceContext->DSSetShader(mpDomainShader, NULL, 0);
		pDeviceContext->PSSetShader(mpPixelShader, NULL, 0);
		pDeviceContext->GSSetShader(mpGeometryShader, NULL, 0);
		pDeviceContext->CSSetShader(mpComputeShader, NULL, 0);
		if (mLayoutList.size() > 0)
		{
			pDeviceContext->IASetInputLayout(mLayoutList.front());
		}
	}

	bool CDX11Shader::createVextexShader(const std::string& fileName, const std::string& strMain, const std::string& strVersion)
	{
		ID3DBlob* errorMessage = nullptr;
		ID3DBlob* vertexShaderBuffer = nullptr;
		// Compile the vertex shader code.
		UINT shaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#if defined( DEBUG ) || defined( _DEBUG )
		shaderFlags |= (D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION | D3DCOMPILE_WARNINGS_ARE_ERRORS);
#endif
		HRESULT result = D3DX11CompileFromFile(fileName.c_str(), NULL, NULL, strMain.c_str(), strVersion.c_str(), shaderFlags, 0, NULL,
			&vertexShaderBuffer, &errorMessage, NULL);
		if (FAILED(result))
		{
			//HR(result);
			// If the shader failed to compile it should have writen something to the error message.	
			MessageBox(GEngine->getMainWnd(), (char*)(errorMessage->GetBufferPointer()), "Missing Shader File", MB_OK);
			return false;
		}


		// Create the vertex shader from the buffer.
		result = mpDevice->getGraphicsDevice()->CreateVertexShader(vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), NULL, &mpVertexShader);
		if (FAILED(result))
		{
			return false;
		}
		DEV_RELEASE(vertexShaderBuffer);
		mVertexShaderName = fileName;
		return true;
	}

	bool CDX11Shader::createVextexShader(const std::string& fileName, const std::string& strMain, const std::string& strVersion, std::vector<INPUT_ELEMENT_DESC> pInputElement)
	{
		ID3DBlob* errorMessage = nullptr;
		ID3DBlob* vertexShaderBuffer = nullptr;
		// Compile the vertex shader code.
		UINT shaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#if defined( DEBUG ) || defined( _DEBUG )
		shaderFlags |= (D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION | D3DCOMPILE_WARNINGS_ARE_ERRORS);
#endif
		HRESULT result = D3DX11CompileFromFile(fileName.c_str(), NULL, NULL, strMain.c_str(), strVersion.c_str(), shaderFlags, 0, NULL,
			&vertexShaderBuffer, &errorMessage, NULL);
		if (FAILED(result))
		{
			//HR(result);
			// If the shader failed to compile it should have writen something to the error message.
			if (errorMessage) {
				std::cout << (char*)(errorMessage->GetBufferPointer());
				MessageBox(GEngine->getMainWnd(), (char*)(errorMessage->GetBufferPointer()), "Missing Shader File", MB_OK);
			}
			else{
				std::cout << "Compile Shader file failed--> " <<fileName.c_str();
				MessageBox(GEngine->getMainWnd(), (char*)(errorMessage->GetBufferPointer()), "Missing Shader File", MB_OK);
			}
			return false;
		}

		// Create the vertex shader from the buffer.
		result = mpDevice->getGraphicsDevice()->CreateVertexShader(vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), NULL, &mpVertexShader);
		if (FAILED(result))
		{
			return false;
		}
		if (pInputElement.size() > 0)
		{
			// Create the vertex input layout.
			ID3D11InputLayout* layout = nullptr;
			result = mpDevice->getGraphicsDevice()->CreateInputLayout(pInputElement.data(), pInputElement.size(), vertexShaderBuffer->GetBufferPointer(),
				vertexShaderBuffer->GetBufferSize(), &layout);
			if (FAILED(result))
			{
				return false;
			}
			mLayoutList.push_back(layout);
		}

		DEV_RELEASE(vertexShaderBuffer);
		mVertexShaderName = fileName;
		return true;
	}

	bool CDX11Shader::createHullShader(const std::string& fileName, const std::string& strMain, const std::string& strVersion)
	{
		ID3DBlob* hullShaderBuffer;
		ID3DBlob* errorMessage = nullptr;
		// Create the hull shader from the buffer.
			// Compile the hull shader code.
		UINT shaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#if defined( DEBUG ) || defined( _DEBUG )
		shaderFlags |= (D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION | D3DCOMPILE_WARNINGS_ARE_ERRORS);
#endif
		HRESULT result = D3DX11CompileFromFile(fileName.c_str(), NULL, NULL, strMain.c_str(), strVersion.c_str(), shaderFlags, 0, NULL,
			&hullShaderBuffer, &errorMessage, NULL);
		if (FAILED(result))
		{
			//HR(result);
			// If the shader failed to compile it should have writen something to the error message.	
			MessageBox(GEngine->getMainWnd(), (char*)(errorMessage->GetBufferPointer()), "Missing Shader File", MB_OK);
			return false;
		}

		result = mpDevice->getGraphicsDevice()->CreateHullShader(hullShaderBuffer->GetBufferPointer(), hullShaderBuffer->GetBufferSize(), NULL, &mpHullShader);
		if (FAILED(result))
		{
			return false;
		}
		DEV_RELEASE(hullShaderBuffer);
		mHullShaderName = fileName;
		return true;
	}

	bool CDX11Shader::createDomainShader(const std::string& fileName, const std::string& strMain, const std::string& strVersion)
	{
		ID3DBlob* domainShaderBuffer;
		ID3DBlob* errorMessage = nullptr;
		// Create the hull shader from the buffer.
			// Compile the hull shader code.
		UINT shaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#if defined( DEBUG ) || defined( _DEBUG )
		shaderFlags |= (D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION | D3DCOMPILE_WARNINGS_ARE_ERRORS);
#endif
		HRESULT result = D3DX11CompileFromFile(fileName.c_str(), NULL, NULL, strMain.c_str(), strVersion.c_str(), shaderFlags, 0, NULL,
			&domainShaderBuffer, &errorMessage, NULL);
		if (FAILED(result))
		{
			//HR(result);
			// If the shader failed to compile it should have writen something to the error message.	
			MessageBox(GEngine->getMainWnd(), (char*)(errorMessage->GetBufferPointer()), "Missing Shader File", MB_OK);
			return false;
		}

		result = mpDevice->getGraphicsDevice()->CreateDomainShader(domainShaderBuffer->GetBufferPointer(), domainShaderBuffer->GetBufferSize(), NULL, &mpDomainShader);
		if (FAILED(result))
		{
			return false;
		}
		DEV_RELEASE(domainShaderBuffer);
		mDomainShaderName = fileName;
		return true;
	}

	bool CDX11Shader::createPixelShader(const std::string& fileName, const std::string& strMain, const std::string& strVersion)
	{
		ID3DBlob* pixelShaderBuffer = nullptr;
		ID3DBlob* errorMessage = nullptr;
		UINT shaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#if defined( DEBUG ) || defined( _DEBUG )
		shaderFlags |= (D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION | D3DCOMPILE_WARNINGS_ARE_ERRORS);
#endif
		// Compile the pixel shader code.
		HRESULT result = D3DX11CompileFromFile(fileName.c_str(), NULL, NULL, strMain.c_str(), strVersion.c_str(), shaderFlags, 0, NULL,
			&pixelShaderBuffer, &errorMessage, NULL);
		if (FAILED(result))
		{
			//	HR(result);
			MessageBox(GEngine->getMainWnd(), (char*)(errorMessage->GetBufferPointer()), "Missing Shader File", MB_OK);
			return false;
		}
		// Create the pixel shader from the buffer.
		result = mpDevice->getGraphicsDevice()->CreatePixelShader(pixelShaderBuffer->GetBufferPointer(), pixelShaderBuffer->GetBufferSize(), NULL, &mpPixelShader);
		if (FAILED(result))
		{
			return false;
		}
		DEV_RELEASE(pixelShaderBuffer);
		mPixelShaderName = fileName;
		return true;
	}

	bool CDX11Shader::createGeometryShader(const std::string& fileName, const std::string& strMain, const std::string& strVersion)
	{
		ID3DBlob* pGeometryShaderBuffer = NULL;
		ID3DBlob* errorbuffer = NULL;
		UINT shaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#if defined( DEBUG ) || defined( _DEBUG )
		shaderFlags |= (D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION | D3DCOMPILE_WARNINGS_ARE_ERRORS);
#endif
		HRESULT result = D3DX11CompileFromFile(fileName.c_str(), NULL, NULL, strMain.c_str(), strVersion.c_str(), shaderFlags, 0, NULL,
			&pGeometryShaderBuffer, &errorbuffer, NULL);

		if (FAILED(result))
		{
			//	HR(result);
			MessageBox(GEngine->getMainWnd(), (char*)(errorbuffer->GetBufferPointer()), "Missing Shader File", MB_OK);
			return false;
		}
#if 0
		result = mpDevice->getDevice()->CreateGeometryShader(pGeometryShaderBuffer->GetBufferPointer(),
			pGeometryShaderBuffer->GetBufferSize(), NULL, &m_pGeometryShader);
#else
		//�������verticesBuffer
		D3D11_SO_DECLARATION_ENTRY SODeclarationEntry[4] =
		{
			{ 0, "POSITION", 0, 0, 4, 0 },
			{ 0, "NORMAL", 0, 0, 3, 0 },
			{ 0, "TEXCOORD", 0, 0, 2, 0 },
			{ 0, "TANGENT", 0, 0, 3, 0 }
		};

		// Create the geometry shader from the buffer & SO declaration
		result = mpDevice->getGraphicsDevice()->CreateGeometryShaderWithStreamOutput(pGeometryShaderBuffer->GetBufferPointer(), pGeometryShaderBuffer->GetBufferSize(), SODeclarationEntry, 4,
			NULL, 0, 0, NULL, &mpGeometryShader);
#endif
		if (FAILED(result))
		{
			return false;
		}
		DEV_RELEASE(pGeometryShaderBuffer);
		mGeometryShaderName = fileName;
		return true;
	}

	bool CDX11Shader::createComputeShader(const std::string& fileName, const std::string& strMain, const std::string& strVersion)
	{
		ID3DBlob* blob = nullptr;
		ID3DBlob* errorbuffer = nullptr;
		std::wstring szFile = L"";
		Foundation::strtowstr(fileName, szFile);
		UINT shaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#if defined( DEBUG ) || defined( _DEBUG )
		shaderFlags |= (D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION | D3DCOMPILE_WARNINGS_ARE_ERRORS);
#endif
		HRESULT hr = D3DX11CompileFromFile(fileName.c_str(), NULL, NULL, strMain.c_str(), strVersion.c_str(), shaderFlags, 0, NULL,
			&blob, &errorbuffer, NULL);
		if (S_OK == hr )
		{
			hr = mpDevice->getGraphicsDevice()->CreateComputeShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, &mpComputeShader);
		}
		else
		{
			if (nullptr != errorbuffer)
			{
				std::cout << (char*)(errorbuffer->GetBufferPointer());
				//	HR(result);
				MessageBox(GEngine->getMainWnd(), (char*)(errorbuffer->GetBufferPointer()), "Missing Shader File", MB_OK);
			}
			else
			{
				std::cout << "failed file" << " " << fileName.c_str();
			}
			return false;
		}
		return S_OK == hr ? true : false;
	}

	bool CDX11Shader::isValidVertexShader()
	{
		return mpVertexShader ? true : false;
	}

	bool CDX11Shader::isValidPixelShader()
	{
		return mpPixelShader ? true : false;
	}

	bool CDX11Shader::isValidHullShader()
	{
		return mpHullShader ? true : false;
	}

	bool CDX11Shader::isValidDomainShader()
	{
		return mpDomainShader ? true : false;
	}

	bool CDX11Shader::isValidGeometryShader()
	{
		return mpGeometryShader ? true : false;
	}

	bool CDX11Shader::isValidComputeShader()
	{
		return mpComputeShader ? true : false;
	}

	void CDX11Shader::replaceTexture(int nRegister, CTexture* pTexture, INT32 nFlag/* = ShaderBufferType::PS*/)
	{
		if (nullptr != pTexture)
		{
			auto iter = mTextureBufferMap.find(nRegister);
			if (iter != mTextureBufferMap.end())
			{
				auto& pair = iter->second;
				WL_DECREASE(std::get<2>(pair));
				auto iterRVM = mTextureBufferResourceViewMap.find(nRegister);
				if (iterRVM != mTextureBufferResourceViewMap.end())
				{
					DEV_RELEASE(iterRVM->second);
				}
			}
			pTexture->addRef();
			mTextureBufferMap[nRegister] = std::tuple<UINT32, std::string, CTexture*>(0, pTexture->getName(), pTexture);
			mTextureBufferResourceViewMap[nRegister] = pTexture->getShaderResViewPtr();
		}
	}

	bool CDX11Shader::addPSConstBuffer(const std::string& name, INT32 size, INT32 nRegister)
	{
		if (size != 0)
		{
			auto iter = mPixelConstBufferMap.find(nRegister);
			if (iter != mPixelConstBufferMap.end())
			{
				auto& pair = iter->second;
				if (size > (INT32)std::get<0>(pair))
				{
					WL_DECREASE(std::get<2>(pair));
				}
				else
				{
					return false;
				}
			}
			// Note that ByteWidth always needs to be a multiple of 16 if using D3D11_BIND_CONSTANT_BUFFER or CreateBuffer will fail.
			D3D11_BUFFER_DESC pixelBufferDesc;
			pixelBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
			pixelBufferDesc.ByteWidth = size;
			pixelBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
			pixelBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
			pixelBufferDesc.MiscFlags = 0;
			pixelBufferDesc.StructureByteStride = 0;
			ID3D11Buffer* pixelConstBuffer = nullptr;
			// Create the constant buffer pointer so we can access the vertex shader constant buffer from within this class.
			HRESULT result = mpDevice->getGraphicsDevice()->CreateBuffer(&pixelBufferDesc, NULL, &pixelConstBuffer);
			if (FAILED(result))
			{
				return false;
			}
			CGpuBuffer* pBuffer = WL_NEW(CGpuBuffer, GpuBuffer);
			pBuffer->bindGpuBuffer(pixelConstBuffer);
			mPixelConstBufferMap[nRegister] = std::tuple<UINT32, std::string, CGpuBuffer*>(size, name, pBuffer);
			mPixelConstBufferStride += size;
			mAllGpuBuffers[name] = pBuffer;
		}
		return true;
	}

	bool CDX11Shader::addDSConstBuffer(const std::string& name, INT32 size, INT32 nRegister)
	{
		if (size != 0)
		{
			auto iter = mDomainConstBufferMap.find(nRegister);
			if (iter != mDomainConstBufferMap.end())
			{
				auto& pair = iter->second;
				if (size > (INT32)std::get<0>(pair))
				{
					WL_DECREASE(std::get<2>(pair));
				}
				else
				{
					return false;
				}
			}
			// Note that ByteWidth always needs to be a multiple of 16 if using D3D11_BIND_CONSTANT_BUFFER or CreateBuffer will fail.
			D3D11_BUFFER_DESC dsBufferDesc;
			dsBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
			dsBufferDesc.ByteWidth = size;
			dsBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
			dsBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
			dsBufferDesc.MiscFlags = 0;
			dsBufferDesc.StructureByteStride = 0;
			ID3D11Buffer* dsConstBuffer = nullptr;
			// Create the constant buffer pointer so we can access the vertex shader constant buffer from within this class.
			HRESULT result = mpDevice->getGraphicsDevice()->CreateBuffer(&dsBufferDesc, NULL, &dsConstBuffer);
			if (FAILED(result))
			{
				return false;
			}
			CGpuBuffer* pBuffer = WL_NEW(CGpuBuffer, GpuBuffer);
			pBuffer->bindGpuBuffer(dsConstBuffer);
			mDomainConstBufferMap[nRegister] = std::tuple<UINT32, std::string, CGpuBuffer*>(size, name, pBuffer);
			mDomainConstBufferStride += size;
			mAllGpuBuffers[name] = pBuffer;
		}
		return true;
	}

	bool CDX11Shader::addHSConstBuffer(const std::string& name, INT32 size, INT32 nRegister)
	{
		if (size != 0)
		{
			auto iter = mHullConstBufferMap.find(nRegister);
			if (iter != mHullConstBufferMap.end())
			{
				auto& pair = iter->second;
				if (size > (INT32)std::get<0>(pair))
				{
					WL_DECREASE(std::get<2>(pair));
				}
				else
				{
					return false;
				}
			}
			// Note that ByteWidth always needs to be a multiple of 16 if using D3D11_BIND_CONSTANT_BUFFER or CreateBuffer will fail.
			D3D11_BUFFER_DESC hullBufferDesc;
			hullBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
			hullBufferDesc.ByteWidth = size;
			hullBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
			hullBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
			hullBufferDesc.MiscFlags = 0;
			hullBufferDesc.StructureByteStride = 0;
			ID3D11Buffer* hullConstBuffer = nullptr;
			// Create the constant buffer pointer so we can access the vertex shader constant buffer from within this class.
			HRESULT result = mpDevice->getGraphicsDevice()->CreateBuffer(&hullBufferDesc, NULL, &hullConstBuffer);
			if (FAILED(result))
			{
				return false;
			}
			CGpuBuffer* pBuffer = WL_NEW(CGpuBuffer, GpuBuffer);
			pBuffer->bindGpuBuffer(hullConstBuffer);
			mHullConstBufferMap[nRegister] = std::tuple<UINT32, std::string, CGpuBuffer*>(size, name, pBuffer);
			mHullConstBufferStride += size;
			mAllGpuBuffers[name] = pBuffer;
		}
		return true;
	}

	bool CDX11Shader::addVSConstBuffer(const std::string& name, INT32 size, INT32 nRegister)
	{
		if (name == "PerCallData" || name == "ShadingData")
		{
			int a = 0;
		}
		if (size != 0)
		{
			auto iter = mVertexConstBufferMap.find(nRegister);
			if (iter != mVertexConstBufferMap.end())
			{
				auto& pair = iter->second;
				if (size > (INT32)std::get<0>(pair))
				{
					WL_DECREASE(std::get<2>(pair));
				}
				else
				{
					return false;
				}
			}
			// Setup the description of the dynamic matrix constant buffer that is in the vertex shader.
			D3D11_BUFFER_DESC matrixBufferDesc;
			matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
			matrixBufferDesc.ByteWidth = size;
			matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
			matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
			matrixBufferDesc.MiscFlags = 0;
			matrixBufferDesc.StructureByteStride = 0;
			ID3D11Buffer* vsConstBuffer = nullptr;
			// Create the constant buffer pointer so we can access the vertex shader constant buffer from within this class.
			HRESULT result = mpDevice->getGraphicsDevice()->CreateBuffer(&matrixBufferDesc, NULL, &vsConstBuffer);
			if (FAILED(result))
			{
				return false;
			}
			CGpuBuffer* pBuffer = WL_NEW(CGpuBuffer, GpuBuffer);
			pBuffer->bindGpuBuffer(vsConstBuffer);
			mVertexConstBufferMap[nRegister] = std::tuple<UINT32, std::string, CGpuBuffer*>(size, name, pBuffer);
			mAllGpuBuffers[name] = pBuffer;
		}
		return true;
	}

	bool CDX11Shader::addGSConstBuffer(const std::string& name, INT32 size, INT32 nRegister)
	{
		if (size != 0)
		{
			auto iter = mGeometryConstBufferMap.find(nRegister);
			if (iter != mGeometryConstBufferMap.end())
			{
				auto& pair = iter->second;
				if (size > (INT32)std::get<0>(pair))
				{
					WL_DECREASE(std::get<2>(pair));
				}
				else
				{
					return false;
				}
			}
			// Note that ByteWidth always needs to be a multiple of 16 if using D3D11_BIND_CONSTANT_BUFFER or CreateBuffer will fail.
			D3D11_BUFFER_DESC GeometryBufferDesc;
			GeometryBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
			GeometryBufferDesc.ByteWidth = size;
			GeometryBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
			GeometryBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
			GeometryBufferDesc.MiscFlags = 0;
			GeometryBufferDesc.StructureByteStride = 0;
			ID3D11Buffer* GSConstBuffer = nullptr;
			// Create the constant buffer pointer so we can access the vertex shader constant buffer from within this class.
			HRESULT result = mpDevice->getGraphicsDevice()->CreateBuffer(&GeometryBufferDesc, NULL, &GSConstBuffer);
			if (FAILED(result))
			{
				return false;
			}
			CGpuBuffer* pBuffer = WL_NEW(CGpuBuffer, GpuBuffer);
			pBuffer->bindGpuBuffer(GSConstBuffer);	
			mGeometryConstBufferMap[nRegister] = std::tuple<UINT32, std::string, CGpuBuffer*>(size, name, pBuffer);
			mGeometryConstBufferStride += size;
			mAllGpuBuffers[name] = pBuffer;
		}
		return true;
	}

	bool CDX11Shader::addCSConstBuffer(const std::string& name, INT32 size, INT32 nRegister, int Usage, UINT32 BindFlags, UINT32 CPUAccessFlags, SUBRESOURCE_DATA* pSubData)
	{
		if (size != 0)
		{
			auto iter = mComputeConstBufferMap.find(nRegister);
			if (iter != mComputeConstBufferMap.end())
			{
				auto& pair = iter->second;
				if (size > (INT32)std::get<0>(pair))
				{
					WL_DECREASE(std::get<2>(pair));
				}
				else
				{
					return false;
				}
			}
			// Setup the description of the dynamic matrix constant buffer that is in the vertex shader.
			D3D11_BUFFER_DESC csBufferDesc;
			csBufferDesc.Usage = static_cast<D3D11_USAGE>(Usage);
			csBufferDesc.ByteWidth = PAD16(size);
			csBufferDesc.BindFlags = BindFlags;
			csBufferDesc.CPUAccessFlags = CPUAccessFlags;
			csBufferDesc.MiscFlags = 0;
			csBufferDesc.StructureByteStride = 0;
			ID3D11Buffer* csConstBuffer = nullptr;
			// Create the constant buffer pointer so we can access the vertex shader constant buffer from within this class.
			HRESULT result = mpDevice->getGraphicsDevice()->CreateBuffer(&csBufferDesc, reinterpret_cast<D3D11_SUBRESOURCE_DATA*>(pSubData), &csConstBuffer);
			if (FAILED(result))
			{
				return false;
			}
			CGpuBuffer* pBuffer = WL_NEW(CGpuBuffer, GpuBuffer);
			pBuffer->bindGpuBuffer(csConstBuffer);
			mComputeConstBufferMap[nRegister] = std::tuple<UINT32, std::string, CGpuBuffer*>(size, name, pBuffer);
			mAllGpuBuffers[name] = pBuffer;
		}
		return true;
	}

	bool CDX11Shader::addTextureConstBufferParam(const std::string& name, INT32 size, INT32 nRegister)
	{
		if (size != 0 && mTextureConstBufferMap.end() == mTextureConstBufferMap.find(nRegister))
		{
			// Setup the description of the dynamic matrix constant buffer that is in the vertex shader.
			D3D11_BUFFER_DESC matrixBufferDesc;
			matrixBufferDesc.Usage = D3D11_USAGE_DEFAULT;
			matrixBufferDesc.ByteWidth = size;
			matrixBufferDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
			matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
			matrixBufferDesc.MiscFlags = 0;
			matrixBufferDesc.StructureByteStride = sizeof(Matrix44);
			ID3D11Buffer* tConstBuffer = nullptr;
			// Create the constant buffer pointer so we can access the vertex shader constant buffer from within this class.
			HRESULT result = mpDevice->getGraphicsDevice()->CreateBuffer(&matrixBufferDesc, NULL, &tConstBuffer);
			if (FAILED(result))
			{
				return false;
			}
			CGpuBuffer* pBuffer = WL_NEW(CGpuBuffer, GpuBuffer);
			pBuffer->bindGpuBuffer(tConstBuffer);
			mTextureConstBufferMap[nRegister] = std::tuple<UINT32, std::string, CGpuBuffer*>(size, name, pBuffer);
			mAllGpuBuffers[name] = pBuffer;

			// Setup the description of the shader resource view.
			ID3D11ShaderResourceView* pShaderResourceView = nullptr;
			D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc;
			shaderResourceViewDesc.Format = DXGI_FORMAT_R32G32B32A32_UINT;
			shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
			shaderResourceViewDesc.Buffer.FirstElement = 0;
			shaderResourceViewDesc.Buffer.NumElements = size / (sizeof(float) * 4);
			// Create the shader resource view.
			result = mpDevice->getGraphicsDevice()->CreateShaderResourceView(tConstBuffer, &shaderResourceViewDesc, &pShaderResourceView);
			if (FAILED(result))
			{
				return false;
			}
			mTextureConstResourceViewMap[nRegister] = pShaderResourceView;
		}
		return true;
	}

	bool CDX11Shader::addStructedParam(const std::string& name, INT32 structSize, INT32 size, INT32 nRegister)
	{
		D3D11_BUFFER_DESC desc;
		desc.ByteWidth = size;
		desc.Usage = D3D11_USAGE_DYNAMIC;
		desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED; // D3D11_RESOURCE_MISC_DRAWINDIRECT_ARGS ?
		desc.StructureByteStride = structSize;

		BYTE* pData = WL_NEW_ARRAY(BYTE, size);
		D3D11_SUBRESOURCE_DATA data;
		data.pSysMem = pData;
		data.SysMemPitch = 0;
		data.SysMemSlicePitch = 0;
		ID3D11Buffer* structBuffer = nullptr;
		bool bResult = false;
		do
		{
			HRESULT result = mpDevice->getGraphicsDevice()->CreateBuffer(&desc, &data, &structBuffer);
			if (S_OK == result)
			{
				CGpuBuffer* pBuffer = WL_NEW(CGpuBuffer, GpuBuffer);
				pBuffer->bindGpuBuffer(structBuffer);
				mStructBufferMap[nRegister] = std::tuple<UINT32, std::string, CGpuBuffer*>(size, name, pBuffer);
				mAllGpuBuffers[name] = pBuffer;
				ID3D11ShaderResourceView* pShaderResourceView = nullptr;
				D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc;
				shaderResourceViewDesc.Format = DXGI_FORMAT_UNKNOWN;
				shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
				shaderResourceViewDesc.Buffer.FirstElement = 0;
				shaderResourceViewDesc.Buffer.NumElements = size / structSize;

				// Create the shader resource view.
				result = mpDevice->getGraphicsDevice()->CreateShaderResourceView(structBuffer, &shaderResourceViewDesc, &pShaderResourceView);
				if (S_OK == result)
				{
					mStructedResourceViewMap[nRegister] = pShaderResourceView;
				}
			}
		} while (false);
		WL_DELETE_ARRAY(pData);
		return bResult;
	}

	bool CDX11Shader::addTextureParam(const std::string& name, INT32 nRegister, ImgFileInfo fileInfo, 
		int Usage, UINT32 BindFlags, UINT32 CPUAccessFlags)
	{
		bool bResult = false;
		if (fileInfo.szPath.length() > 4)
		{
			auto pTexture = dynamic_cast<CTexture*>(GEngine->createResource(ResourceType::Texture));

			if (nullptr != pTexture)
			{
				pTexture->initialise(fileInfo);
				mTextureBufferMap[nRegister] = std::tuple<UINT32, std::string, CTexture*>(fileInfo.nFileSize, name, pTexture);
				mTextureBufferResourceViewMap[nRegister] = pTexture->getShaderResViewPtr();
				bResult = true;
			}
			return bResult;
		}
		do
		{
			ID3D11ShaderResourceView* pShaderResourceView = nullptr;
			D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc;
			if (1 == fileInfo.nType)
			{
				BYTE* pData = WL_NEW_ARRAY(BYTE, fileInfo.nSliceSize);
				D3D11_SUBRESOURCE_DATA subresData;
				subresData.pSysMem = pData;
				subresData.SysMemPitch = fileInfo.nSliceSize;
				subresData.SysMemSlicePitch = fileInfo.nSliceSize;

				auto pTexture = GEngine->createResource<CTexture>(ResourceType::Texture);
				pTexture->initialiseTexture2D(fileInfo.nWidth, fileInfo.nHeight, fileInfo.nFormat,
					static_cast<D3D11_USAGE>(BindFlags),
					CPUAccessFlags,
					Usage, 0);

				mTextureBufferMap[nRegister] = std::tuple<UINT32, std::string, CTexture*>(fileInfo.nFileSize, name, pTexture);
				//mTexture1DResourceViewMap[nRegister] = pTexture1D;

				// Setup the description of the shader resource view.		
				shaderResourceViewDesc.Format = static_cast<DXGI_FORMAT>(fileInfo.nFormat);
				shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE1D;
				shaderResourceViewDesc.Texture1D.MipLevels = pTexture->mDesc1D.MipLevels;
				shaderResourceViewDesc.Texture1D.MostDetailedMip = 0;

				// Create the shader resource view.
				auto result = mpDevice->getGraphicsDevice()->CreateShaderResourceView(pTexture->getResourcePtr(), &shaderResourceViewDesc, &pShaderResourceView);
				WL_DELETE_ARRAY(pData);

				if (FAILED(result))
				{
					break;
				}
			}
			else if (2 == fileInfo.nType || 4 == fileInfo.nType)
			{
				auto pTexture = GEngine->createResource<CTexture>(ResourceType::Texture);
				pTexture->initialiseTexture2D(fileInfo.nWidth, fileInfo.nHeight, fileInfo.nFormat,
					static_cast<D3D11_USAGE>(BindFlags),
					CPUAccessFlags,
					Usage, 0);
				mTextureBufferMap[nRegister] = std::tuple<UINT32, std::string, CTexture*>(fileInfo.nFileSize, name, pTexture);

				// Setup the description of the shader resource view.		
				shaderResourceViewDesc.Format = static_cast<DXGI_FORMAT>(fileInfo.nFormat);

				shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
				shaderResourceViewDesc.Texture2D.MipLevels = 1;
				shaderResourceViewDesc.Texture2D.MostDetailedMip = 0;

				if (4 == fileInfo.nType)
				{
					shaderResourceViewDesc.ViewDimension = D3D_SRV_DIMENSION_TEXTURECUBE;
					shaderResourceViewDesc.TextureCube.MipLevels = 1;
					shaderResourceViewDesc.TextureCube.MostDetailedMip = 0;
				}

				// Create the shader resource view.
				auto result = mpDevice->getGraphicsDevice()->CreateShaderResourceView(pTexture->getResourcePtr(), &shaderResourceViewDesc, &pShaderResourceView);
				if (FAILED(result))
				{
					break;
				}
			}
			else if (3 == fileInfo.nType)
			{
				auto pTexture = GEngine->createResource<CTexture>(ResourceType::Texture);
				pTexture->initialiseTexture2D(fileInfo.nWidth, fileInfo.nHeight, fileInfo.nFormat,
					static_cast<D3D11_USAGE>(BindFlags),
					CPUAccessFlags,
					Usage, 0);
				mTextureBufferMap[nRegister] = std::tuple<UINT32, std::string, CTexture*>(fileInfo.nFileSize, name, pTexture);

				// Setup the description of the shader resource view.		
				shaderResourceViewDesc.Format = static_cast<DXGI_FORMAT>(fileInfo.nFormat);
				shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE3D;
				shaderResourceViewDesc.Texture3D.MipLevels = pTexture->mDesc3D.MipLevels;
				shaderResourceViewDesc.Texture3D.MostDetailedMip = 0;

				// Create the shader resource view.
				auto result = mpDevice->getGraphicsDevice()->CreateShaderResourceView(pTexture->getResourcePtr(), &shaderResourceViewDesc, &pShaderResourceView);
				if (FAILED(result))
				{
					break;
				}

			}
			mTextureBufferResourceViewMap[nRegister] = pShaderResourceView;
			bResult = true;
		} while (false);
		return bResult;
	}

	bool CDX11Shader::addTypeBufferParam(const std::string& name, INT32 size, INT32 nRegister)
	{
		bool bResult = false;
		do
		{
			// Setup the description of the dynamic matrix constant buffer that is in the vertex shader.
			D3D11_BUFFER_DESC matrixBufferDesc;
			matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
			matrixBufferDesc.ByteWidth = size;
			matrixBufferDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
			matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
			matrixBufferDesc.MiscFlags = 0;
			matrixBufferDesc.StructureByteStride = 0;
			ID3D11Buffer* pTypeBuffer = nullptr;
			// Create the constant buffer pointer so we can access the vertex shader constant buffer from within this class.
			HRESULT result = mpDevice->getGraphicsDevice()->CreateBuffer(&matrixBufferDesc, NULL, &pTypeBuffer);
			if (FAILED(result))
			{
				return false;
			}
			CGpuBuffer* pBuffer = WL_NEW(CGpuBuffer, GpuBuffer);
			pBuffer->bindGpuBuffer(pTypeBuffer);
			mTypeBufferMap[nRegister] = std::tuple<UINT32, std::string, CGpuBuffer*>(size, name, pBuffer);
			mAllGpuBuffers[name] = pBuffer;
			//mTexture1DResourceViewMap[nRegister] = pTexture1D;

			// Setup the description of the shader resource view.		
			ID3D11ShaderResourceView* pShaderResourceView = nullptr;
			D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc;
			shaderResourceViewDesc.Format = DXGI_FORMAT_R32G32B32A32_UINT;
			shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
			shaderResourceViewDesc.Buffer.FirstElement = 0;
			shaderResourceViewDesc.Buffer.NumElements = size / (sizeof(float) * 4);

			// Create the shader resource view.
			result = mpDevice->getGraphicsDevice()->CreateShaderResourceView(pTypeBuffer, &shaderResourceViewDesc, &pShaderResourceView);
			if (FAILED(result))
			{
				break;
			}
			mTypeResourceViewMap[nRegister] = pShaderResourceView;
			bResult = true;
		} while (false);
		return bResult;
	}

	void CDX11Shader::setSampler(int nRegister, SamplerState* pSampler)
	{
		if (nullptr != pSampler && pSampler != mTextureSample[nRegister])
		{
			DEV_RELEASE(mTextureSample[nRegister])mTextureSample;
			mTextureSample[nRegister] = pSampler;
		}
	}

	SamplerState* CDX11Shader::getSampler(int nRegister) const
	{
		auto iter = mTextureSample.find(nRegister);
		if (iter != mTextureSample.end())
		{
			return iter->second;
		}
		return nullptr;
	}

}