#include "MaterialMgr.h"
#include "Material.h"

namespace ML
{
	bool CMaterialMgr::Init()
	{
		// P0 stage has no preset assets; keep the interface for the future (default/WorldGrid etc. can be built in Init).
		return true;
	}

	void CMaterialMgr::Shutdown()
	{
		mRegistry.clear();
		mDefaultMaterial = nullptr;
	}

	bool CMaterialMgr::Register(const std::string& Name, CMaterial* InMaterial)
	{
		if (nullptr == InMaterial || Name.empty())
		{
			return false;
		}
		// Later registration overrides earlier ones: handy for tests / level overrides
		mRegistry[Name] = InMaterial;
		return true;
	}

	CMaterial* CMaterialMgr::Find(const std::string& Name) const
	{
		auto It = mRegistry.find(Name);
		return (It != mRegistry.end()) ? It->second : nullptr;
	}

	void CMaterialMgr::SetDefaultMaterial(CMaterial* InMaterial)
	{
		mDefaultMaterial = InMaterial;
	}

	CMaterial* CMaterialMgr::GetDefaultMaterial() const
	{
		return mDefaultMaterial;
	}
}
