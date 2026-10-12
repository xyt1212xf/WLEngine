#pragma once
#include <d3d12.h>
#include "Resource.h"

namespace ML
{ 
	class CDX12Shader : public CResource
	{
	public:
		CDX12Shader();
		virtual ~CDX12Shader();
		bool IsValid() const { return mpByteCode != nullptr; }
		ID3DBlob* ExtractRootSignature() const;
		bool CompileFromFile(const std::string& FilePath, const std::string& EntryPoint, const std::string& Target);

		// 取编译产物, 填 PSO 的 D3D12_SHADER_BYTECODE 用
		// 调用点限定在 DX12 层文件内(如 DX12PipelineState.cpp)
		D3D12_SHADER_BYTECODE GetShaderByteCode() const;

	private:
		ID3DBlob* mpByteCode = nullptr;   // 编译产物(字节码)
	};

}