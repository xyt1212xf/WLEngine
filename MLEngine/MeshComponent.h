#pragma once
#include "PrimitiveComponent.h"
namespace ML
{
	class CMaterial;

	class MLENGINE_API CMeshComponent : public CPrimitiveComponent
    {
    public:
        CMeshComponent();
        ~CMeshComponent();

		// UE: UMeshComponent::OverrideMaterials.
		// An empty array means "use the mesh asset's own material slots" - the
		// same convention UE uses, where the mesh owns StaticMaterials and the
		// component only overrides them slot by slot.
		void SetOverrideMaterial(int32 SlotIndex, CMaterial* InMaterial)
		{
			if (SlotIndex < 0)
			{
				return;
			}
			if (OverrideMaterials.size() <= (size_t)SlotIndex)
			{
				OverrideMaterials.resize((size_t)SlotIndex + 1, nullptr);
			}
			OverrideMaterials[(size_t)SlotIndex] = InMaterial;
		}

		// Returns nullptr when this slot is not overridden (or the override is null).
		CMaterial* GetOverrideMaterial(int32 SlotIndex) const
		{
			if (SlotIndex < 0 || (size_t)SlotIndex >= OverrideMaterials.size())
			{
				return nullptr;
			}
			return OverrideMaterials[(size_t)SlotIndex];
		}

		const std::vector<CMaterial*>& GetOverrideMaterials() const { return OverrideMaterials; }

    protected:
		std::vector<CMaterial*> OverrideMaterials;
    };
}
