#pragma once
#include "Vector.h"
namespace ML
{
	class CMeshVertex
	{
	public:
		CMeshVertex(); 
		~CMeshVertex();
		Vec3F Position;
		Vec3F Normal;
		Vec2F UV;
	};
}