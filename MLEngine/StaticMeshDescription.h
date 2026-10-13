#pragma once
#include "Object.h"
#include "Vector.h"
#include "MeshDescription.h"

namespace ML
{
	class MLENGINE_API CStaticMeshDescription : public CObject
	{
	public:
		CStaticMeshDescription() = default;
		virtual ~CStaticMeshDescription();

		void CreateCube(Vec3F Center, Vec3F HalfExtents);

		// Access to the owned topology (read/write for editors and importers).
		FMeshDescription& GetMeshDescription() { return OwnedMeshDescription; }
		const FMeshDescription& GetMeshDescription() const { return OwnedMeshDescription; }

	protected:
		FMeshDescription OwnedMeshDescription;
	};
}
