#pragma once
#include "DX12Shader.h"
namespace ML
{	class CShader : public CDX12Shader
	{
	public:
		CShader() = default;
		virtual ~CShader();		
	};
}
