#include "BoundingSphere.h"
#include "ModelInstance.h"

namespace WL
{
	CBoundingSphere::CBoundingSphere()
	{

	}
	CBoundingSphere::~CBoundingSphere()
	{
		WL_DECREASE(mpModelInstance);
	}
}
