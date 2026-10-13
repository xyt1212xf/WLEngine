#include "ShaderMgr.h"
#include "GlobalShader.h"

namespace ML
{
	bool CShaderMgr::Init()
	{
		// Internally delegates to CGlobalShaderMap. P1 may switch to owned storage.
		return CGlobalShaderMap::InitGlobalShaders();
	}

	void CShaderMgr::Shutdown()
	{
		CGlobalShaderMap::DestroyGlobalShaders();
	}

	CGlobalShader* CShaderMgr::GetShader(const std::string& Name) const
	{
		return CGlobalShaderMap::GetShader(Name);
	}
}
