#pragma once
#include "Mgr.h"
#include <string>
#include "Refcount.h"

namespace ML
{
	class CShader;
	class CGlobalShader;

	// Engine shader subsystem.
	// P0 stage: internally delegates to the static singleton CGlobalShaderMap
	// (whose mTypes is populated statically by the IMPLEMENT_GLOBAL_SHADER macro).
	// CShaderMgr brings it into CEngine's Init/Shutdown flow.
	//
	// P1 upgrade: move mTypes into CShaderMgr and kill CGlobalShaderMap's static state.
	class MLENGINE_API CShaderMgr : public CMgr
	{
	public:
		virtual const char* GetName() const override { return "ShaderMgr"; }

		// Compiles all shaders registered via IMPLEMENT_GLOBAL_SHADER.
		virtual bool Init() override;

		// Releases all compiled artifacts.
		virtual void Shutdown() override;

		// Finds a compiled shader by name (CGlobalShader subclass or CShader instance).
		CGlobalShader* GetShader(const std::string& Name) const;

		// Template version: GetShader<CSolidColorVS>().
		template<class T>
		T* GetShader() const
		{
			return static_cast<T*>(GetShader(T::StaticShaderName()));
		}
	};
}
