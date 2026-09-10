#include "MathLib.h"
#include "Common.h"
#include "WLEngine.h"

namespace WL
{
	namespace Math
	{

		float vec3Dot(const Vec3F* pV1, const Vec3F* pV2)
		{
#ifdef _DEBUG
			if (nullptr == pV1 || nullptr == pV2)
			{
				return -1;
			}
#endif // DEBUG
			return pV1->x * pV2->x + pV1->y * pV2->y + pV1->z * pV2->z;
		}

		float vec3DotRadian(const Vec3F* pV1, const Vec3F* pV2)
		{
			float fRadian = vec3Dot(pV1, pV2);
			return 	acos(fRadian);
		}

		float vec3DotAngle(const Vec3F* pV1, const Vec3F* pV2)
		{
			float fAngle = vec3Dot(pV1, pV2);
			return radianToAngle(acos(fAngle));
		}

		void vec3Cross(InOut Vec3F* pOut, const Vec3F* pV1, const Vec3F* pV2)
		{
#ifdef _DEBUG
			if (nullptr == pOut || nullptr == pV1 || nullptr == pV2)
			{
				return;
			}
#endif // DEBUG
			pOut->x = pV1->y * pV2->z - pV1->z * pV2->y;
			pOut->y = pV1->z * pV2->x - pV1->x * pV2->z;
			pOut->z = pV1->x * pV2->y - pV1->y * pV2->x;
		}

		void buildMatrixLookAtLH(InOut SMatrix44* pOut, const Vec3F* Eye, const Vec3F* LookAt, const Vec3F* Up)
		{
#ifdef _DEBUG
			if (nullptr == pOut || nullptr == Eye || nullptr == LookAt || nullptr == Up)
			{
				return;
			}
#endif // DEBUG
			Vec3F xAxis;
			Vec3F yAxis;

			Vec3F zAxis = *LookAt - *Eye;
			zAxis.normalize();
			vec3Cross(&xAxis, Up, &zAxis);

			xAxis.normalize();

			vec3Cross(&yAxis, &zAxis, &xAxis);

			pOut->_11 = xAxis.x;
			pOut->_21 = xAxis.y;
			pOut->_31 = xAxis.z;
			pOut->_41 = -vec3Dot(&xAxis, Eye);
			pOut->_12 = yAxis.x;
			pOut->_22 = yAxis.y;
			pOut->_32 = yAxis.z;
			pOut->_42 = -vec3Dot(&yAxis, Eye);
			pOut->_13 = zAxis.x;
			pOut->_23 = zAxis.y;
			pOut->_33 = zAxis.z;
			pOut->_43 = -vec3Dot(&zAxis, Eye);
			pOut->_14 = 0.0f;
			pOut->_24 = 0.0f;
			pOut->_34 = 0.0f;
			pOut->_44 = 1.0f;
		}

		void buildMatrixPerspectiveFovLH(InOut SMatrix44* pOut, float fovy, float Aspect, float fNear, float fFar)
		{
#ifdef _DEBUG
			if (nullptr == pOut)
			{
				return;
			}
#endif // DEBUG
			float angle = tan(fovy * 0.5f);
			float s1 = 1 / (Aspect * angle);
			float s2 = 1 / angle;
			float a = fFar / (fFar - fNear);
			float b = -(fNear * fFar) / (fFar - fNear);

			pOut->m[0][0] = s1;
			pOut->m[1][0] = 0;
			pOut->m[2][0] = 0;
			pOut->m[3][0] = 0;

			pOut->m[0][1] = 0;
			pOut->m[1][1] = s2;
			pOut->m[2][1] = 0;
			pOut->m[3][1] = 0;

			pOut->m[0][2] = 0;
			pOut->m[1][2] = 0;
			pOut->m[2][2] = a;
			pOut->m[3][2] = b;

			pOut->m[0][3] = 0;
			pOut->m[1][3] = 0;
			pOut->m[2][3] = 1;
			pOut->m[3][3] = 0;
		}

		void buildOrthographicMatrixLH(InOut SMatrix44* pOut, float fWidth, float fHeight, float fNear, float fFar)
		{
#ifdef _DEBUG
			if (nullptr == pOut)
			{
				return;
			}
#endif // DEBUG
			pOut->m[0][0] = 2 / fWidth;
			pOut->m[0][1] = 0;
			pOut->m[0][2] = 0;
			pOut->m[0][3] = 0;

			pOut->m[1][0] = 0;
			pOut->m[1][1] = 2 / fHeight;
			pOut->m[1][2] = 0;
			pOut->m[1][3] = 0;

			pOut->m[2][0] = 0;
			pOut->m[2][1] = 0;
			pOut->m[2][2] = 1 / (fFar - fNear);
			pOut->m[2][3] = 0;

			pOut->m[3][0] = 0;
			pOut->m[3][1] = 0;
			pOut->m[3][2] = fNear / (fNear - fFar);
			pOut->m[3][3] = 1;
		}

		void computeTriNormal(const Vec3F& v1, const Vec3F& v2, const Vec3F& v3, InOut Vec3F& normalVec)
		{
			Vec3F tmpV1;
			tmpV1.x = v1.x - v2.x;
			tmpV1.y = v1.y - v2.y;
			tmpV1.z = v1.z - v2.z;
			Vec3F tmpV2;
			tmpV2.x = v1.x - v3.x;
			tmpV2.y = v1.y - v3.y;
			tmpV2.z = v1.z - v3.z;

			vec3Cross(&normalVec, &tmpV1, &tmpV2);
			normalVec.normalize();
		}

		Vec3F vec3Projective(const Vec3F& vFrom, const Vec3F& vTo)
		{
			float fValue = vec3Dot(&vFrom, &vTo);
			float l = vTo.length();
			return vTo * fValue / (l * l);
		}

		Vec3F vec3NomalizeProjective(const Vec3F& vFrom, const Vec3F& vTo)
		{
			float fValue = vec3Dot(&vFrom, &vTo);
			return vTo * fValue;
		}

		Vec3F lerp(const Vec3F& vFrom, const Vec3F& vTo, float t)
		{
			return Vec3F(vFrom.x + t *(vTo.x - vFrom.x), vFrom.y + t * (vTo.y - vFrom.y), vFrom.z + t * (vTo.z-vFrom.z));
		}

		CQuaternion lerp(const CQuaternion& q1, const CQuaternion& q2, float t)
		{
			return CQuaternion(q1.mx + t * (q2.mx - q1.mx), q1.my + t * (q2.my - q1.my), q1.mz + t * (q2.mz - q1.mz), q1.mw + t * (q2.mw - q1.mw));
		}


		float vec3Radian(const Vec3F& pFrom, const Vec3F& pTo)
		{
			Vec3F pFromNormal = pFrom;
			pFromNormal.normalize();
			Vec3F pToNormal = pTo;
			pToNormal.normalize();
			float fValue = acos(vec3Dot(&pFromNormal, &pToNormal));
			Vec3F v;
			vec3Cross(&v, &pFrom, &pTo);
			if (v.z < 0)
			{
				return -fValue;
			}
			else
			{
				return fValue;
			}
		}

		float vec3Angle(const Vec3F& pFrom, const Vec3F& pTo, const Vec3F& vDirNormal)
		{
			Vec3F pFromNormal = pFrom;
			pFromNormal.normalize();
			Vec3F pToNormal = pTo;
			pToNormal.normalize();
			float fValue = radianToAngle(acos(vec3Dot(&pFromNormal, &pToNormal)));
			Vec3F v;
			Vec3F vFrom = pFrom;
			vFrom.normalize();
			Vec3F vTo = pTo;
			vTo.normalize();

			vec3Cross(&v, &vFrom, &vTo);
			float z = vec3Dot(&v, &vDirNormal);
			if (z < 0)
			{
				return -fValue;
			}
			else
			{
				return fValue;
			}
		}

		void matrixTranspose(Out SMatrix44* pOut, In const SMatrix44* pIn)
		{
			pOut->m[0][0] = pIn->m[0][0];
			pOut->m[0][1] = pIn->m[1][0];
			pOut->m[0][2] = pIn->m[2][0];
			pOut->m[0][3] = pIn->m[3][0];

			pOut->m[1][0] = pIn->m[0][1];
			pOut->m[1][1] = pIn->m[1][1];
			pOut->m[1][2] = pIn->m[2][1];
			pOut->m[1][3] = pIn->m[3][1];

			pOut->m[2][0] = pIn->m[0][2];
			pOut->m[2][1] = pIn->m[1][2];
			pOut->m[2][2] = pIn->m[2][2];
			pOut->m[2][3] = pIn->m[3][2];

			pOut->m[3][0] = pIn->m[0][3];
			pOut->m[3][1] = pIn->m[1][3];
			pOut->m[3][2] = pIn->m[2][3];
			pOut->m[3][3] = pIn->m[3][3];
		}

		void matrixTranslation(InOut SMatrix44* pInOut, float x, float y, float z)
		{
			pInOut->m[3][0] = x;
			pInOut->m[3][1] = y;
			pInOut->m[3][2] = z;
		}

		void matrixScale(InOut SMatrix44* pInOut, float x, float y, float z)
		{
			pInOut->m[0][0] *= x;
			pInOut->m[1][1] *= y;
			pInOut->m[2][2] *= z;
		}

		void matrixRotationX(InOut SMatrix44* pOut, float radian)
		{
			float cs = (float)cos(radian);
			float ss = (float)sin(radian);

			pOut->m[0][0] = 1.0f;
			pOut->m[0][1] = 0;
			pOut->m[0][2] = 0;
			pOut->m[0][3] = 0;

			pOut->m[1][0] = 0;
			pOut->m[1][1] = cs;
			pOut->m[1][2] = -ss;
			pOut->m[1][3] = 0;

			pOut->m[2][0] = 0;
			pOut->m[2][1] = ss;
			pOut->m[2][2] = cs;
			pOut->m[2][3] = 0;

			pOut->m[3][0] = 0;
			pOut->m[3][1] = 0;
			pOut->m[3][2] = 0;
			pOut->m[3][3] = 1.0f;

		}

		void matrixRotationY(InOut SMatrix44* pOut, float radian)
		{
			float cs = (float)cos(radian);
			float ss = (float)sin(radian);

			pOut->m[0][0] = cs;
			pOut->m[0][1] = 0;
			pOut->m[0][2] = ss;
			pOut->m[0][3] = 0;

			pOut->m[1][0] = 0;
			pOut->m[1][1] = 1.0f;
			pOut->m[1][2] = 0;
			pOut->m[1][3] = 0;

			pOut->m[2][0] = -ss;
			pOut->m[2][1] = 0;
			pOut->m[2][2] = cs;
			pOut->m[2][3] = 0;

			pOut->m[3][0] = 0;
			pOut->m[3][1] = 0;
			pOut->m[3][2] = 0;
			pOut->m[3][3] = 1.0f;
		}

		void matrixRotationZ(InOut SMatrix44* pOut, float radian)
		{
			float cs = (float)cos(radian);
			float ss = (float)sin(radian);

			pOut->m[0][0] = cs;
			pOut->m[0][1] = -ss;
			pOut->m[0][2] = 0;
			pOut->m[0][3] = 0;

			pOut->m[1][0] = ss;
			pOut->m[1][1] = cs;
			pOut->m[1][2] = 0;
			pOut->m[1][3] = 0;

			pOut->m[2][0] = 0;
			pOut->m[2][1] = 0;
			pOut->m[2][2] = 1.0f;
			pOut->m[2][3] = 0;

			pOut->m[3][0] = 0;
			pOut->m[3][1] = 0;
			pOut->m[3][2] = 0;
			pOut->m[3][3] = 1.0f;

		}

		void vec3RotationX(InOut Vec3F* pVec3, float radian)
		{
			float cs = (float)cos(radian);
			float ss = (float)sin(radian);

			float y = cs * pVec3->y - ss * pVec3->z;
			float z = ss * pVec3->y + cs * pVec3->z;

			pVec3->y = y;
			pVec3->z = z;
		}

		void vec3RotationY(InOut Vec3F* pVec3, float radian)
		{
			float cs = (float)cos(radian);
			float ss = (float)sin(radian);

			float x = cs * pVec3->x + ss * pVec3->z;
			float z = -ss * pVec3->x + cs * pVec3->z;

			pVec3->x = x;
			pVec3->z = z;
		}

		void vec3RotationZ(InOut Vec3F* pVec3, float radian)
		{
			float cs = (float)cos(radian);
			float ss = (float)sin(radian);

			float x = cs * pVec3->x - ss * pVec3->y;
			float y = ss * pVec3->x + cs * pVec3->y;

			pVec3->x = x;
			pVec3->y = y;
		}

		void matrixRotationEulerian(InOut SMatrix44* pOut, const Vec3F& radian)
		{
			SMatrix44 x, y, z;
			Math::matrixIdentity(&x);
			Math::matrixIdentity(&y);
			Math::matrixIdentity(&z);
			matrixRotationX(&x, radian.x);
			matrixRotationY(&y, radian.y);
			matrixRotationZ(&z, radian.z);
			*pOut = x * y * z;
		}

		void vec3Transform(InOut Vec3F* pInOut, In const SMatrix44* pMatrix)
		{
			float x = pMatrix->m[0][0] * pInOut->x + pMatrix->m[1][0] * pInOut->y + pMatrix->m[2][0] * pInOut->z + pMatrix->m[3][0];
			float y = pMatrix->m[0][1] * pInOut->x + pMatrix->m[1][1] * pInOut->y + pMatrix->m[2][1] * pInOut->z + pMatrix->m[3][1];
			float z = pMatrix->m[0][2] * pInOut->x + pMatrix->m[1][2] * pInOut->y + pMatrix->m[2][2] * pInOut->z + pMatrix->m[3][2];

			pInOut->x = x;
			pInOut->y = y;
			pInOut->z = z;
		}

		void vec4Transform(InOut Vec4F* pInOut, In const SMatrix44* pMatrix)
		{
			float x = pMatrix->m[0][0] * pInOut->x + pMatrix->m[1][0] * pInOut->y + pMatrix->m[2][0] * pInOut->z + pMatrix->m[3][0];
			float y = pMatrix->m[0][1] * pInOut->x + pMatrix->m[1][1] * pInOut->y + pMatrix->m[2][1] * pInOut->z + pMatrix->m[3][1];
			float z = pMatrix->m[0][2] * pInOut->x + pMatrix->m[1][2] * pInOut->y + pMatrix->m[2][2] * pInOut->z + pMatrix->m[3][2];
			float w = pMatrix->m[0][3] * pInOut->x + pMatrix->m[1][3] * pInOut->y + pMatrix->m[2][3] * pInOut->z + pMatrix->m[3][3];

			pInOut->x = x;
			pInOut->y = y;
			pInOut->z = z;
			pInOut->w = w;
		}

		void matrixScaling(InOut SMatrix44* pOut, float x, float y, float z)
		{
			pOut->m[0][0] = x;
			pOut->m[1][1] = y;
			pOut->m[2][2] = z;
		}

		void matrixIdentity(InOut SMatrix44* pOut)
		{
			pOut->m[0][0] = 1.0f;
			pOut->m[0][1] = 0;
			pOut->m[0][2] = 0;
			pOut->m[0][3] = 0;

			pOut->m[1][0] = 0;
			pOut->m[1][1] = 1.0f;
			pOut->m[1][2] = 0;
			pOut->m[1][3] = 0;

			pOut->m[2][0] = 0;
			pOut->m[2][1] = 0;
			pOut->m[2][2] = 1.0f;
			pOut->m[2][3] = 0;

			pOut->m[3][0] = 0;
			pOut->m[3][1] = 0;
			pOut->m[3][2] = 0;
			pOut->m[3][3] = 1.0f;
		}
		float determinant(const SMatrix44* pMatrix)
		{
			return 0;
		}

		bool matrixInverse(InOut SMatrix44* pOut, IN const SMatrix44* pMatrix)
		{
			float det = (pMatrix->m[0][0] * pMatrix->m[1][1] - pMatrix->m[0][1] * pMatrix->m[1][0]) * (pMatrix->m[2][2] * pMatrix->m[3][3] - pMatrix->m[2][3] * pMatrix->m[3][2]) -
				(pMatrix->m[0][0] * pMatrix->m[1][2] - pMatrix->m[0][2] * pMatrix->m[1][0]) * (pMatrix->m[2][1] * pMatrix->m[3][3] - pMatrix->m[2][3] * pMatrix->m[3][1]) +
				(pMatrix->m[0][0] * pMatrix->m[1][3] - pMatrix->m[0][3] * pMatrix->m[1][0]) * (pMatrix->m[2][1] * pMatrix->m[3][2] - pMatrix->m[2][2] * pMatrix->m[3][1]) +
				(pMatrix->m[0][1] * pMatrix->m[1][2] - pMatrix->m[0][2] * pMatrix->m[1][1]) * (pMatrix->m[2][0] * pMatrix->m[3][3] - pMatrix->m[2][3] * pMatrix->m[3][0]) -
				(pMatrix->m[0][1] * pMatrix->m[1][3] - pMatrix->m[0][3] * pMatrix->m[1][1]) * (pMatrix->m[2][0] * pMatrix->m[3][2] - pMatrix->m[2][2] * pMatrix->m[3][0]) +
				(pMatrix->m[0][2] * pMatrix->m[1][3] - pMatrix->m[0][3] * pMatrix->m[1][2]) * (pMatrix->m[2][0] * pMatrix->m[3][1] - pMatrix->m[2][1] * pMatrix->m[3][0]);
			if (fabs(det) <= ROUNDING_ERROR_f32)
			{
				return false;
			}
			det = 1 / det;
			SMatrix44 m44;
			SMatrix44* pResult = pOut;
			bool bCopyMT = false;
			if( pResult != pMatrix)
			{
				pResult = pOut;
			}
			else
			{
				bCopyMT = true;
				pResult = &m44;
			}
			pResult->m[0][0] = det * (pMatrix->m[1][1] * (pMatrix->m[2][2] * pMatrix->m[3][3] - pMatrix->m[2][3] * pMatrix->m[3][2]) +
				pMatrix->m[1][2] * (pMatrix->m[2][3] * pMatrix->m[3][1] - pMatrix->m[2][1] * pMatrix->m[3][3]) +
				pMatrix->m[1][3] * (pMatrix->m[2][1] * pMatrix->m[3][2] - pMatrix->m[2][2] * pMatrix->m[3][1]));

			pResult->m[0][1] = det * (pMatrix->m[2][1] * (pMatrix->m[0][2] * pMatrix->m[3][3] - pMatrix->m[0][3] * pMatrix->m[3][2]) +
				pMatrix->m[2][2] * (pMatrix->m[0][3] * pMatrix->m[3][1] - pMatrix->m[0][1] * pMatrix->m[3][3]) +
				pMatrix->m[2][3] * (pMatrix->m[0][1] * pMatrix->m[3][2] - pMatrix->m[0][2] * pMatrix->m[3][1]));

			pResult->m[0][2] = det * (pMatrix->m[3][1] * (pMatrix->m[0][2] * pMatrix->m[1][3] - pMatrix->m[0][3] * pMatrix->m[1][2]) +
				pMatrix->m[3][2] * (pMatrix->m[0][3] * pMatrix->m[1][1] - pMatrix->m[0][1] * pMatrix->m[1][3]) +
				pMatrix->m[3][3] * (pMatrix->m[0][1] * pMatrix->m[1][2] - pMatrix->m[0][2] * pMatrix->m[1][1]));

			pResult->m[0][3] = det * (pMatrix->m[0][1] * (pMatrix->m[1][3] * pMatrix->m[2][2] - pMatrix->m[1][2] * pMatrix->m[2][3]) +
				pMatrix->m[0][2] * (pMatrix->m[1][1] * pMatrix->m[2][3] - pMatrix->m[1][3] * pMatrix->m[2][1]) +
				pMatrix->m[0][3] * (pMatrix->m[1][2] * pMatrix->m[2][1] - pMatrix->m[1][1] * pMatrix->m[2][2]));

			pResult->m[1][0] = det * (pMatrix->m[1][2] * (pMatrix->m[2][0] * pMatrix->m[3][3] - pMatrix->m[2][3] * pMatrix->m[3][0]) +
				pMatrix->m[1][3] * (pMatrix->m[2][2] * pMatrix->m[3][0] - pMatrix->m[2][0] * pMatrix->m[3][2]) +
				pMatrix->m[1][0] * (pMatrix->m[2][3] * pMatrix->m[3][2] - pMatrix->m[2][2] * pMatrix->m[3][3]));

			pResult->m[1][1] = det * (pMatrix->m[2][2] * (pMatrix->m[0][0] * pMatrix->m[3][3] - pMatrix->m[0][3] * pMatrix->m[3][0]) +
				pMatrix->m[2][3] * (pMatrix->m[0][2] * pMatrix->m[3][0] - pMatrix->m[0][0] * pMatrix->m[3][2]) +
				pMatrix->m[2][0] * (pMatrix->m[0][3] * pMatrix->m[3][2] - pMatrix->m[0][2] * pMatrix->m[3][3]));

			pResult->m[1][2] = det * (pMatrix->m[3][2] * (pMatrix->m[0][0] * pMatrix->m[1][3] - pMatrix->m[0][3] * pMatrix->m[1][0]) +
				pMatrix->m[3][3] * (pMatrix->m[0][2] * pMatrix->m[1][0] - pMatrix->m[0][0] * pMatrix->m[1][2]) +
				pMatrix->m[3][0] * (pMatrix->m[0][3] * pMatrix->m[1][2] - pMatrix->m[0][2] * pMatrix->m[1][3]));

			pResult->m[1][3] = det * (pMatrix->m[0][2] * (pMatrix->m[1][3] * pMatrix->m[2][0] - pMatrix->m[1][0] * pMatrix->m[2][3]) +
				pMatrix->m[0][3] * (pMatrix->m[1][0] * pMatrix->m[2][2] - pMatrix->m[1][2] * pMatrix->m[2][0]) +
				pMatrix->m[0][0] * (pMatrix->m[1][2] * pMatrix->m[2][3] - pMatrix->m[1][3] * pMatrix->m[2][2]));

			pResult->m[2][0] = det * (pMatrix->m[1][3] * (pMatrix->m[2][0] * pMatrix->m[3][1] - pMatrix->m[2][1] * pMatrix->m[3][0]) +
				pMatrix->m[1][0] * (pMatrix->m[2][1] * pMatrix->m[3][3] - pMatrix->m[2][3] * pMatrix->m[3][1]) +
				pMatrix->m[1][1] * (pMatrix->m[2][3] * pMatrix->m[3][0] - pMatrix->m[2][0] * pMatrix->m[3][3]));

			pResult->m[2][1] = det * (pMatrix->m[2][3] * (pMatrix->m[0][0] * pMatrix->m[3][1] - pMatrix->m[0][1] * pMatrix->m[3][0]) +
				pMatrix->m[2][0] * (pMatrix->m[0][1] * pMatrix->m[3][3] - pMatrix->m[0][3] * pMatrix->m[3][1]) +
				pMatrix->m[2][1] * (pMatrix->m[0][3] * pMatrix->m[3][0] - pMatrix->m[0][0] * pMatrix->m[3][3]));

			pResult->m[2][2] = det * (pMatrix->m[3][3] * (pMatrix->m[0][0] * pMatrix->m[1][1] - pMatrix->m[0][1] * pMatrix->m[1][0]) +
				pMatrix->m[3][0] * (pMatrix->m[0][1] * pMatrix->m[1][3] - pMatrix->m[0][3] * pMatrix->m[1][1]) +
				pMatrix->m[3][1] * (pMatrix->m[0][3] * pMatrix->m[1][0] - pMatrix->m[0][0] * pMatrix->m[1][3]));

			pResult->m[2][3] = det * (pMatrix->m[0][3] * (pMatrix->m[1][1] * pMatrix->m[2][0] - pMatrix->m[1][0] * pMatrix->m[2][1]) +
				pMatrix->m[0][0] * (pMatrix->m[1][3] * pMatrix->m[2][1] - pMatrix->m[1][1] * pMatrix->m[2][3]) +
				pMatrix->m[0][1] * (pMatrix->m[1][0] * pMatrix->m[2][3] - pMatrix->m[1][3] * pMatrix->m[2][0]));

			pResult->m[3][0] = det * (pMatrix->m[1][0] * (pMatrix->m[2][2] * pMatrix->m[3][1] - pMatrix->m[2][1] * pMatrix->m[3][2]) +
				pMatrix->m[1][1] * (pMatrix->m[2][0] * pMatrix->m[3][2] - pMatrix->m[2][2] * pMatrix->m[3][0]) +
				pMatrix->m[1][2] * (pMatrix->m[2][1] * pMatrix->m[3][0] - pMatrix->m[2][0] * pMatrix->m[3][1]));

			pResult->m[3][1] = det * (pMatrix->m[2][0] * (pMatrix->m[0][2] * pMatrix->m[3][1] - pMatrix->m[0][1] * pMatrix->m[3][2]) +
				pMatrix->m[2][1] * (pMatrix->m[0][0] * pMatrix->m[3][2] - pMatrix->m[0][2] * pMatrix->m[3][0]) +
				pMatrix->m[2][2] * (pMatrix->m[0][1] * pMatrix->m[3][0] - pMatrix->m[0][0] * pMatrix->m[3][1]));

			pResult->m[3][2] = det * (pMatrix->m[3][0] * (pMatrix->m[0][2] * pMatrix->m[1][1] - pMatrix->m[0][1] * pMatrix->m[1][2]) +
				pMatrix->m[3][1] * (pMatrix->m[0][0] * pMatrix->m[1][2] - pMatrix->m[0][2] * pMatrix->m[1][0]) +
				pMatrix->m[3][2] * (pMatrix->m[0][1] * pMatrix->m[1][0] - pMatrix->m[0][0] * pMatrix->m[1][1]));

			pResult->m[3][3] = det * (pMatrix->m[0][0] * (pMatrix->m[1][1] * pMatrix->m[2][2] - pMatrix->m[1][2] * pMatrix->m[2][1]) +
				pMatrix->m[0][1] * (pMatrix->m[1][2] * pMatrix->m[2][0] - pMatrix->m[1][0] * pMatrix->m[2][2]) +
				pMatrix->m[0][2] * (pMatrix->m[1][0] * pMatrix->m[2][1] - pMatrix->m[1][1] * pMatrix->m[2][0]));
			if (bCopyMT)
			{
				memcpy(pOut, &m44, sizeof(SMatrix44));
			}
			return true;
		}

		void matrixToEulerian_XYZ(In SMatrix44* pIn, Out float& x, Out float& y, Out float& z)
		{
			y = -asinf(pIn->m[0][2]);
			z = atan2f(pIn->m[0][1], pIn->m[0][0]);
			x = atan2f(pIn->m[1][2], pIn->m[2][2]);
		}

		void matrixToEulerian_ZYX(In SMatrix44* pIn, Out float& x, Out float& y, Out float& z)
		{
			y = -asinf(pIn->m[2][0]);
			x = atan2f(pIn->m[2][1], pIn->m[2][2]);
			z = atan2f(pIn->m[1][0], pIn->m[0][0]);
		}


		Vec4F matrixMulVec4(In const SMatrix44& matrix, In const Vec4F& v)
		{
			Vec4F res;
			res.x = matrix.m[0][0] * v.x + matrix.m[1][0] * v.y + matrix.m[2][0] * v.z + matrix.m[3][0] * v.w;
			res.y = matrix.m[0][1] * v.x + matrix.m[1][1] * v.y + matrix.m[2][1] * v.z + matrix.m[3][1] * v.w;
			res.z = matrix.m[0][2] * v.x + matrix.m[1][2] * v.y + matrix.m[2][2] * v.z + matrix.m[3][2] * v.w;
			res.w = matrix.m[0][3] * v.x + matrix.m[1][3] * v.y + matrix.m[2][3] * v.z + matrix.m[3][3] * v.w;
			return res;
		}


		Vec3F matrixMulVec3(In const SMatrix44& matrix, In const Vec3F& v)
		{
			Vec3F res;
			res.x = matrix.m[0][0] * v.x + matrix.m[1][0] * v.y + matrix.m[2][0] * v.z + matrix.m[3][0];
			res.y = matrix.m[0][1] * v.x + matrix.m[1][1] * v.y + matrix.m[2][1] * v.z + matrix.m[3][1];
			res.z = matrix.m[0][2] * v.x + matrix.m[1][2] * v.y + matrix.m[2][2] * v.z + matrix.m[3][2];
			return res;
		}

		//--------------------------
		// Plane
		//--------------------------
		float planeDotCoord(In SPlane& plane, In const Vec3F& v)
		{
			return plane.n.x * v.x + plane.n.y * v.y + plane.n.z * v.z + plane.d;
		}

		void PlaneFromPoints(InOut SPlane& plane, const Vec3F& p0, const Vec3F& p1, const Vec3F& p2)
		{
			auto&& vDir0 = p1 - p0;
			auto&& vDir1 = p2 - p0;
			Vec3F normal;
			vec3Cross(&plane.n, &vDir0, &vDir1);
			plane.n.normalize();
			plane.d = -vec3Dot(&plane.n, &p0);
		}

		void PlaneFromPointNormal(InOut SPlane& plane, const Vec3F& p, const Vec3F& normal)
		{
			plane.n = normal;
			plane.d = -vec3Dot(&plane.n, &p);
		}

		bool intersectTriangle(const float* orig, const float* dir, const float* v0, const float* v1, const float* v2, float* t, float* u, float* v)
		{
			// Find vectors for two edges sharing vert0
			Vec3F edge1 = Vec3F(v1) - Vec3F(v0);
			Vec3F edge2 = Vec3F(v2) - Vec3F(v0);

			// Begin calculating determinant - also used to calculate U parameter
			Vec3F pvec;
			Vec3F dir3f(dir);
			vec3Cross(&pvec, &dir3f, &edge2);

			// If determinant is near zero, ray lies in plane of triangle
			float det = vec3Dot(&edge1, &pvec);

			Vec3F tvec;
			if (det > 0)
			{
				tvec = Vec3F(orig) - Vec3F(v0);
			}
			else
			{
				tvec = Vec3F(v0) - Vec3F(orig);
				det = -det;
			}

			if (det < 0.0001f)
			{
				return false;
			}
			// Calculate U parameter and test bounds
			*u = vec3Dot(&tvec, &pvec);
			if (*u < 0.0f || *u > det)
			{
				return false;
			}

			// Prepare to test V parameter
			Vec3F qvec;
			vec3Cross(&qvec, &tvec, &edge1);

			// Calculate V parameter and test bounds
			*v = vec3Dot(&dir3f, &qvec);
			if (*v < 0.0f || *u + *v > det)
			{
				return false;
			}
			// Calculate t, scale parameters, ray intersects triangle
			*t = vec3Dot(&edge2, &qvec);
			float fInvDet = 1.0f / det;
			*t *= fInvDet;
			*u *= fInvDet;
			*v *= fInvDet;
			return true;
		}

		bool intersectTriangle(const float* orig, const float* dir, const float* v0, const float* v1, const float* v2, float* t)
		{
			// Find vectors for two edges sharing vert0
			Vec3F edge1 = Vec3F(v1) - Vec3F(v0);
			Vec3F edge2 = Vec3F(v2) - Vec3F(v0);

			// Begin calculating determinant - also used to calculate U parameter
			Vec3F pvec;
			Vec3F dir3f(dir);
			vec3Cross(&pvec, &dir3f, &edge2);

			// If determinant is near zero, ray lies in plane of triangle
			float det = vec3Dot(&edge1, &pvec);

			Vec3F tvec;
			if (det > 0)
			{
				tvec = Vec3F(orig) - Vec3F(v0);
			}
			else
			{
				tvec = Vec3F(v0) - Vec3F(orig);
				det = -det;
			}

			if (det < 0.0001f)
			{
				return false;
			}
			// Calculate U parameter and test bounds
			float u = vec3Dot(&tvec, &pvec);
			if (u < 0.0f || u > det)
			{
				return false;
			}

			// Prepare to test V parameter
			Vec3F qvec;
			vec3Cross(&qvec, &tvec, &edge1);

			// Calculate V parameter and test bounds
			float v = vec3Dot(&dir3f, &qvec);
			if (v < 0.0f || u + v > det)
			{
				return false;
			}
			// Calculate t, scale parameters, ray intersects triangle
			*t = vec3Dot(&edge2, &qvec) / det;
			return true;
		}

		bool inSideAABB(const CAABBox& bigAABB, const CAABBox& smallAABB)
		{
			if (smallAABB.mMinPos.x < bigAABB.mMinPos.x)
			{
				return false;
			}
			if (smallAABB.mMinPos.y < bigAABB.mMinPos.y)
			{
				return false;
			}
			if (smallAABB.mMinPos.z < bigAABB.mMinPos.z)
			{
				return false;
			}

			if (smallAABB.mMaxPos.x > bigAABB.mMaxPos.x)
			{
				return false;
			}
			if (smallAABB.mMaxPos.y > bigAABB.mMaxPos.y)
			{
				return false;
			}
			if (smallAABB.mMaxPos.z > bigAABB.mMaxPos.z)
			{
				return false;
			}
			return true;
		}

	}
}