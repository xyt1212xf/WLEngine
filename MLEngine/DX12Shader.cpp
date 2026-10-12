#include "DX12Shader.h"
#include <d3dcompiler.h>

namespace ML
{

	CDX12Shader::CDX12Shader()
	{

	}

	CDX12Shader::~CDX12Shader()
	{
		SafeRelease(mpByteCode);

	}

	ID3DBlob* CDX12Shader::ExtractRootSignature() const
	{
		if (nullptr == mpByteCode)
		{
			return nullptr;
		}
		ID3DBlob* pRootSig = nullptr;
		D3DGetBlobPart( mpByteCode->GetBufferPointer(),
						mpByteCode->GetBufferSize(),
						D3D_BLOB_ROOT_SIGNATURE,
						0,
						&pRootSig);
		return pRootSig;
	}

	D3D12_SHADER_BYTECODE CDX12Shader::GetShaderByteCode() const
	{
		D3D12_SHADER_BYTECODE Code = {};
		Code.pShaderBytecode	= mpByteCode ? mpByteCode->GetBufferPointer() : nullptr;
		Code.BytecodeLength		= mpByteCode ? mpByteCode->GetBufferSize() : 0;
		return Code;
	}

	bool CDX12Shader::CompileFromFile(const std::string& FilePath, const std::string& EntryPoint, const std::string& Target)
	{
		SafeRelease(mpByteCode);
		
		// 调试期带调试信息、跳过优化, 方便定位
		UINT Flags = D3DCOMPILE_ENABLE_STRICTNESS;
#ifdef _DEBUG
		Flags |= D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif
		
		// D3DCompileFromFile 只接受宽字符路径(ASCII 路径直接扩宽即可)
		std::wstring WidePath(FilePath.begin(), FilePath.end());
		
		ID3DBlob* pError = nullptr;
		HRESULT hr = D3DCompileFromFile(
					WidePath.c_str(),
					nullptr,							// 宏定义(以后做 permutation 再启用)
					D3D_COMPILE_STANDARD_FILE_INCLUDE,	// 支持 #include "xxx.ush"
					EntryPoint.c_str(),
					Target.c_str(),						// "vs_5_0" / "ps_5_0"
					Flags,
					0,
					&mpByteCode,
					&pError);
		
		if (FAILED(hr))
		{
			// 编译错误信息打到 VS 输出窗口
			if (pError)
			{
				OutputDebugStringA(static_cast<const char*>(pError->GetBufferPointer()));
				SafeRelease(pError);
			}
			mpByteCode = nullptr;
			return false;
		}
		return true;
	}
}