#pragma once
#include <d3d12.h>
#include "Object.h"

namespace ML
{ 
	// Known deviation: UE's FShader is NOT a UObject (shaders live in
	// FShaderMap / FShaderCodeLibrary, outside the object system). P0 keeps
	// CDX12Shader as a CObject so it shares the engine's name/lifetime
	// handling; revisit when the shader map moves to FShader ownership.
	class CDX12Shader : public CObject
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