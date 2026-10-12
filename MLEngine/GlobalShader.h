#pragma once
#include "Shader.h"
namespace ML
{
	class CGlobalShader;

	using FGlobalShaderCreateFn = CGlobalShader * (*)();

	struct FGlobalShaderType
	{
		std::string				Name;		
		std::string				SourceFile;	
		std::string				EntryPoint;	
		std::string				Target;		
		FGlobalShaderCreateFn	CreateFn = nullptr;
	};

	class CGlobalShader : public CShader
	{
		friend class CGlobalShaderMap;
	public:
		const std::string& GetShaderName() const { return mShaderName; }

	private:
		std::string mShaderName;
	};

	class CGlobalShaderMap
	{
	public:
		static bool RegisterType(const FGlobalShaderType& InType);

		static bool InitGlobalShaders();

		static void DestroyGlobalShaders();

		static CGlobalShader* GetShader(const std::string& InName);

		template<class T>
		static T* GetShader()
		{
			return static_cast<T*>(GetShader(T::StaticShaderName()));
		}

	private:
		static CGlobalShaderMap& Get();

		std::unordered_map<std::string, FGlobalShaderType>	mTypes;		
		std::unordered_map<std::string, CGlobalShader*>		mInstances;	
	};


#define DECLARE_GLOBAL_SHADER(ClassName) \
	public: static const char* StaticShaderName() { return #ClassName; }
#define IMPLEMENT_GLOBAL_SHADER(ClassName, sourceFile, entryPoint, shaderTarget) \
	static CGlobalShader* _CreateGlobalShader_##ClassName() { return new ClassName(); } \
	static const bool _RegisteredGlobalShader_##ClassName = []() { \
		ML::FGlobalShaderType _Type; \
		_Type.Name		= #ClassName; \
		_Type.SourceFile= sourceFile; \
		_Type.EntryPoint= entryPoint; \
		_Type.Target	= shaderTarget; \
		_Type.CreateFn	= &_CreateGlobalShader_##ClassName; \
		return CGlobalShaderMap::RegisterType(_Type); \
	}();
}

