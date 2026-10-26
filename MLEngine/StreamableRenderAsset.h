#pragma once
#include "Object.h"

namespace ML
{
	// UE: UStreamableRenderAsset - a CObject whose GPU payload can be dropped
	// and streamed back in (mesh LODs, texture mips).
	//
	// This is a FACET on top of CObject, not a general-purpose "resource" base.
	// UE's hierarchy is UObject -> UStreamableRenderAsset -> UStaticMesh, while
	// UMaterialInterface hangs off UObject directly (materials do not stream)
	// and UWorld / ULevel / ULevelStreaming are plain UObjects.
	//
	// Note on what is NOT here: UE carries no type-tag enum, no UUID and no
	// status field on this class. Identity is the class itself (UClass* /
	// IsA<T> at runtime); load state lives on the streaming system, not on the
	// asset. The old CResource's ResourceType/sResType/mTypeName/mUUID/
	// ResourceStatus baggage was removed along with its factory/mgr shells -
	// they had no live callers.
	class MLENGINE_API CStreamableRenderAsset : public CObject
	{
	public:
		CStreamableRenderAsset() = default;
		virtual ~CStreamableRenderAsset() = default;
	};
}
