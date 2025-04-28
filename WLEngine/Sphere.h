#pragma once
#include "Vector.h"
namespace WL
{
	class CSphere
	{
	public:
		CSphere() = default;
		~CSphere() = default;

		CSphere(const Vec3F& vec, float fRadius);
		CSphere(const CSphere& vec);
		CSphere(CSphere&& vec);
		
		void setPosition(const Vec3F& vec);
		void setRadius(float fRadius);
		float getRadius()const;
		const Vec3F& getPosition()const;

	private:
		Vec3F mOrigin = { 0, 0, 0 };
		float mRadius = 0;
	};
}