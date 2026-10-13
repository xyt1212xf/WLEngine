#include "GlobalShader.h"
namespace ML
{
	static bool TryCompileFromSearchPaths(CShader* pShader, const std::string& SourceFile, const std::string& EntryPoint, const std::string& Target)
	{
		static const char* SearchDirs[] =
		{
			"Shaders/",
			"",
			"../../../MLEngine/Shaders/",
		};
		for (const char* Dir : SearchDirs)
		{
			if (pShader->CompileFromFile(std::string(Dir) + SourceFile, EntryPoint, Target))
			{
				return true;
			}
		}
		return false;
	}

	CGlobalShaderMap& CGlobalShaderMap::Get()
	{
		static CGlobalShaderMap Instance;
		return Instance;
	}

	bool CGlobalShaderMap::RegisterType(const FGlobalShaderType& InType)
	{
		CGlobalShaderMap& Map = Get();
		if (Map.mTypes.find(InType.Name) != Map.mTypes.end())
		{
			return false;	
		}
		Map.mTypes[InType.Name] = InType;
		return true;
	}

	bool CGlobalShaderMap::InitGlobalShaders()
	{
		CGlobalShaderMap& Map = Get();
		bool bAllOk = true;

		for (auto& Pair : Map.mTypes)
		{
			const FGlobalShaderType& Type = Pair.second;

			if (Map.mInstances.find(Type.Name) != Map.mInstances.end())
			{
				continue;
			}
			if (nullptr == Type.CreateFn)
			{
				bAllOk = false;
				continue;
			}

			CGlobalShader* pShader = Type.CreateFn();
			if (!TryCompileFromSearchPaths(pShader, Type.SourceFile, Type.EntryPoint, Type.Target))
			{
				OutputDebugStringA(("GlobalShader compile failed: " + Type.Name + "\n").c_str());
				delete pShader;
				bAllOk = false;
				continue;
			}
			pShader->mShaderName = Type.Name;
			Map.mInstances[Type.Name] = pShader;
		}
		return bAllOk;
	}

	void CGlobalShaderMap::DestroyGlobalShaders()
	{
		CGlobalShaderMap& Map = Get();
		for (auto& Pair : Map.mInstances)
		{
			delete Pair.second;
		}
		Map.mInstances.clear();
	}

	CGlobalShader* CGlobalShaderMap::GetShader(const std::string& InName)
	{
		CGlobalShaderMap& Map = Get();
		auto It = Map.mInstances.find(InName);
		return (It != Map.mInstances.end()) ? It->second : nullptr;
	}
}