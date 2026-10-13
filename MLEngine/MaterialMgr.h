#pragma once
#include "Mgr.h"
#include "Common.h"
#include <string>
#include <unordered_map>
#include "Refcount.h"

namespace ML
{
	class CMaterial;

	// Engine material subsystem (instance class, owned by CEngine, inherits CMgr).
	// UE counterpart: FMaterialResourceMap + the engine's GEngine->MaterialResources.
	// Difference: this mgr only stores weak refs (raw pointers); lifetime is managed
	// by GraphicPlug / the asset system. Real ownership lives in CStaticMesh's MaterialSlots.
	class MLENGINE_API CMaterialMgr : public CMgr
	{
	public:
		virtual const char* GetName() const override { return "MaterialMgr"; }

		virtual bool Init() override;     // P0: nothing to do yet, keep the interface
		virtual void Shutdown() override; // clears the registry + default material

		// Register/override by name.
		bool Register(const std::string& Name, CMaterial* InMaterial);
		CMaterial* Find(const std::string& Name) const;

		// Engine default material: used for WorldGrid / fallback / unassigned sections.
		// Created and InitRHI'd by GraphicPlug::Initialise after all GlobalShaders are compiled.
		void SetDefaultMaterial(CMaterial* InMaterial);
		CMaterial* GetDefaultMaterial() const;

	private:
		std::unordered_map<std::string, CMaterial*>	mRegistry;
		CMaterial*	mDefaultMaterial = nullptr;
	};
}
