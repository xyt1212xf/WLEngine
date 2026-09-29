#pragma once
#include "Object.h"
#include "Vector.h"
#include "MeshDescription.h"

namespace ML
{
	class CStaticMeshDescription : public CObject
	{
	public:
		CStaticMeshDescription() = default;
		virtual ~CStaticMeshDescription();

		void CreateCube(Vec3F Coneter, Vec3F HalfExtents);

	//	FVertexArray& Vertices() { return GetMeshDescription().Vertices(); }
	//	const FVertexArray& Vertices() const { return GetMeshDescription().Vertices(); }

	protected:
		FMeshDescription OwnedMeshDescription;
	};
}