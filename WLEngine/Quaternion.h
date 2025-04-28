#pragma once
#include "Vector.h"

namespace WL
{
	class CQuaternion
	{
	public:
		CQuaternion() = default;
		CQuaternion(float x, float y, float z, float w)
		{
			mx = x;
			my = y;
			mz = z;
			mw = w;
		}

		explicit CQuaternion(float* array)
		{
			mx = array[0];
			my = array[1];
			mz = array[2];
			mw = array[3];
		}

		explicit CQuaternion(const float* array)
		{
			mx = array[0];
			my = array[1];
			mz = array[2];
			mw = array[3];
		}

		//! Add operator
		CQuaternion operator+(const CQuaternion& other) const;

		//! Multiplication operator
		CQuaternion operator*(const CQuaternion& other) const;

		//! Multiplication operator with scalar
		CQuaternion operator*(float s) const;

		//! Multiplication operator with scalar
		CQuaternion& operator*=(float s);

		void inverse();
		void identity();

		void toEuler(Vec3F& euler);
		// sets new quaternion
		CQuaternion& set(float x, float y, float z, float w);

		// sets new quaternion based on euler angles
		CQuaternion& set(float x, float y, float z);

		// normalizes the quaternion
		CQuaternion& normalize();
	

	public:
		float mx = 0;
		float my = 0;
		float mz = 0;
		float mw = 0;
	};
}

