#include "Matrix.h"
namespace WL
{

	SMatrix44::SMatrix44(const float* fStream)
	{
		memcpy(&_11, fStream, sizeof(SMatrix44));
	}

	SMatrix44::SMatrix44(const SMatrix44& mat)
	{
		memcpy(&_11, &mat, sizeof(SMatrix44));
	}

	SMatrix44::SMatrix44(float f11, float f12, float f13, float f14,
					   float f21, float f22, float f23, float f24,
					   float f31, float f32, float f33, float f34,
					   float f41, float f42, float f43, float f44)
	{
		_11 = f11; _12 = f12; _13 = f13; _14 = f14;
		_21 = f21; _22 = f22; _23 = f23; _24 = f24;
		_31 = f31; _32 = f32; _33 = f33; _34 = f34;
		_41 = f41; _42 = f42; _43 = f43; _44 = f44;
	}


	SMatrix44::SMatrix44(SMatrix44&& mat)
	{
		memcpy(&_11, &mat, sizeof(SMatrix44));
	}

	void SMatrix44::operator*=(const SMatrix44& mat)
	{
#ifdef WIN64
		m[0][0] = m[0][0] * mat.m[0][0] + m[0][1] * mat.m[1][0] + m[0][2] * mat.m[2][0] + m[0][3] * mat.m[3][0];
		m[0][1] = m[0][0] * mat.m[0][1] + m[0][1] * mat.m[1][1] + m[0][2] * mat.m[2][1] + m[0][3] * mat.m[3][1];
		m[0][2] = m[0][0] * mat.m[0][2] + m[0][1] * mat.m[1][2] + m[0][2] * mat.m[2][2] + m[0][3] * mat.m[3][2];
		m[0][3] = m[0][0] * mat.m[0][3] + m[0][1] * mat.m[1][3] + m[0][2] * mat.m[2][3] + m[0][3] * mat.m[3][3];

		m[1][0] = m[1][0] * mat.m[0][0] + m[1][1] * mat.m[1][0] + m[1][2] * mat.m[2][0] + m[1][3] * mat.m[3][0];
		m[1][1] = m[1][0] * mat.m[0][1] + m[1][1] * mat.m[1][1] + m[1][2] * mat.m[2][1] + m[1][3] * mat.m[3][1];
		m[1][2] = m[1][0] * mat.m[0][2] + m[1][1] * mat.m[1][2] + m[1][2] * mat.m[2][2] + m[1][3] * mat.m[3][2];
		m[1][3] = m[1][0] * mat.m[0][3] + m[1][1] * mat.m[1][3] + m[1][2] * mat.m[2][3] + m[1][3] * mat.m[3][3];

		m[2][0] = m[2][0] * mat.m[0][0] + m[2][1] * mat.m[1][0] + m[2][2] * mat.m[2][0] + m[2][3] * mat.m[3][0];
		m[2][1] = m[2][0] * mat.m[0][1] + m[2][1] * mat.m[1][1] + m[2][2] * mat.m[2][1] + m[2][3] * mat.m[3][1];
		m[2][2] = m[2][0] * mat.m[0][2] + m[2][1] * mat.m[1][2] + m[2][2] * mat.m[2][2] + m[2][3] * mat.m[3][2];
		m[2][3] = m[2][0] * mat.m[0][3] + m[2][1] * mat.m[1][3] + m[2][2] * mat.m[2][3] + m[2][3] * mat.m[3][3];

		m[3][0] = m[3][0] * mat.m[0][0] + m[3][1] * mat.m[1][0] + m[3][2] * mat.m[2][0] + m[3][3] * mat.m[3][0];
		m[3][1] = m[3][0] * mat.m[0][1] + m[3][1] * mat.m[1][1] + m[3][2] * mat.m[2][1] + m[3][3] * mat.m[3][1];
		m[3][2] = m[3][0] * mat.m[0][2] + m[3][1] * mat.m[1][2] + m[3][2] * mat.m[2][2] + m[3][3] * mat.m[3][2];
		m[3][3] = m[3][0] * mat.m[0][3] + m[3][1] * mat.m[1][3] + m[3][2] * mat.m[2][3] + m[3][3] * mat.m[3][3];

#else

		const float* pData = &mat.m[0][0];
		__asm
		{
			mov esi, this;
			mov edi, pData;

			movups xmm4, [edi];
			movups xmm5, [edi + 16];
			movups xmm6, [edi + 32];
			movups xmm7, [edi + 48];

			mov ecx, 4;

		LOOPIT:
			movss xmm0, [esi];
			shufps xmm0, xmm0, 0;
			mulps xmm0, xmm4;

			movss xmm1, [esi + 4];
			shufps xmm1, xmm1, 0;
			mulps xmm1, xmm5;

			movss xmm2, [esi + 8];
			shufps xmm2, xmm2, 0;
			mulps xmm2, xmm6;

			movss xmm3, [esi + 12];
			shufps xmm3, xmm3, 0;
			mulps xmm3, xmm7;

			addps xmm0, xmm1;
			addps xmm0, xmm2;
			addps xmm0, xmm3;

			movups[esi], xmm0;

			add	esi, 16;
			loop LOOPIT;
		}
#endif
	}

	void SMatrix44::operator+=(const SMatrix44& mat) 
	{
#ifdef WIN64
		_11 += mat._11, _12 += mat._12, _13 += mat._13, _14 += mat._14,
			_21 += mat._21, _22 += mat._22, _23 += mat._23, _24 += mat._24,
			_31 += mat._31, _32 += mat._32, _33 += mat._33, _34 += mat._34,
			_41 += mat._41, _42 += mat._42, _43 += mat._43, _44 += mat._44;

		
#else
		const float* pData = &mat.m[0][0];
		__asm
		{
			mov  eax, this;
			mov  edi, pData;
			movups	xmm0, [eax];
			movups	xmm1, [eax + 16];
			movups	xmm2, [eax + 32];
			movups	xmm3, [eax + 48];

			movups	xmm4, [edi];
			movups	xmm5, [edi + 16];
			movups	xmm6, [edi + 32];
			movups	xmm7, [edi + 48];

			addps	xmm0, xmm4;
			addps	xmm1, xmm5;
			addps	xmm2, xmm6;
			addps	xmm3, xmm7;

			movups[eax], xmm0;
			movups[eax + 16], xmm1;
			movups[eax + 32], xmm2;
			movups[eax + 48], xmm3;
		}
#endif
	}

	void SMatrix44::operator-=(const SMatrix44& mat) 
	{
#ifdef WIN64
		_11 -= mat._11, _12 -= mat._12, _13 -= mat._13, _14 -= mat._14,
			_21 -= mat._21, _22 -= mat._22, _23 -= mat._23, _24 -= mat._24,
			_31 -= mat._31, _32 -= mat._32, _33 -= mat._33, _34 -= mat._34,
			_41 -= mat._41, _42 -= mat._42, _43 -= mat._43, _44 -= mat._44;
#else
		const float* pData = &mat.m[0][0];
		__asm
		{
			mov  eax, this;
			mov  edi, pData;
			movups	xmm0, [eax];
			movups	xmm1, [eax + 16];
			movups	xmm2, [eax + 32];
			movups	xmm3, [eax + 48];

			movups	xmm4, [edi];
			movups	xmm5, [edi + 16];
			movups	xmm6, [edi + 32];
			movups	xmm7, [edi + 48];

			subps	xmm0, xmm4;
			subps	xmm1, xmm5;
			subps	xmm2, xmm6;
			subps	xmm3, xmm7;

			movups[eax], xmm0;
			movups[eax + 16], xmm1;
			movups[eax + 32], xmm2;
			movups[eax + 48], xmm3;
		}
#endif
	}

	void SMatrix44::operator/=(float f) 
	{
#ifdef WIN64
		float fInv = 1.0f / f;
		_11 *= fInv; _12 *= fInv; _13 *= fInv; _14 *= fInv;
		_21 *= fInv; _22 *= fInv; _23 *= fInv; _24 *= fInv;
		_31 *= fInv; _32 *= fInv; _33 *= fInv; _34 *= fInv;
		_41 *= fInv; _42 *= fInv; _43 *= fInv; _44 *= fInv;

#else

		float fInv = 1.0f / f;
		__asm
		{
			mov  eax, this;
			movss xmm0, fInv;
			shufps xmm0, xmm0, 0;

			movups	xmm1, [eax];
			movups	xmm2, [eax + 16];
			movups	xmm3, [eax + 32];
			movups	xmm4, [eax + 48];

			mulps	xmm1, xmm0;
			mulps	xmm2, xmm0;
			mulps	xmm3, xmm0;
			mulps	xmm4, xmm0;

			movups[eax], xmm1;
			movups[eax + 16], xmm2;
			movups[eax + 32], xmm3;
			movups[eax + 48], xmm4;
		}
#endif
	}

	void SMatrix44::operator*=(float f) 
	{
#ifdef WIN64
		_11 *= f; _12 *= f; _13 *= f; _14 *= f;
		_21 *= f; _22 *= f; _23 *= f; _24 *= f;
		_31 *= f; _32 *= f; _33 *= f; _34 *= f;
		_41 *= f; _42 *= f; _43 *= f; _44 *= f;

#else
		__asm
		{
			mov  eax, this;
			movss xmm0, f;
			shufps xmm0, xmm0, 0;

			movups	xmm1, [eax];
			movups	xmm2, [eax + 16];
			movups	xmm3, [eax + 32];
			movups	xmm4, [eax + 48];

			mulps	xmm1, xmm0;
			mulps	xmm2, xmm0;
			mulps	xmm3, xmm0;
			mulps	xmm4, xmm0;

			movups[eax], xmm1;
			movups[eax + 16], xmm2;
			movups[eax + 32], xmm3;
			movups[eax + 48], xmm4;
		}
#endif
	}

	void SMatrix44::zero()
	{
		memset(&m, 0, sizeof(SMatrix44));
	}

	void SMatrix44::transpose()
	{
		m[0][1] = m[1][0];
		m[0][2] = m[2][0];
		m[0][3] = m[3][0];

		m[1][0] = m[0][1];
		m[1][2] = m[2][1];
		m[1][3] = m[3][1];

		m[2][0] = m[0][2];
		m[2][1] = m[1][2];
		m[2][3] = m[3][2];

		m[3][0] = m[0][3];
		m[3][1] = m[1][3];
		m[3][2] = m[2][3];
	}

	void* SMatrix44::data()
	{
		return m;
	}

	void SMatrix44::operator=(SMatrix44&& mat)
	{
		memcpy(&m, &mat, sizeof(SMatrix44));
	}

	void SMatrix44::operator=(const SMatrix44& mat)
	{
		memcpy(&m, &mat, sizeof(SMatrix44));
	}

	bool SMatrix44::operator==(const SMatrix44& mat) const
	{
		for( int i = 0; i < 4; ++i )
		{
			if (mat.m[0][0] != m[0][0] ||
				mat.m[0][1] != m[0][1] ||
				mat.m[0][2] != m[0][2] ||
				mat.m[0][3] != m[0][3])
			{
				return false;
			}
			if (mat.m[1][0] != m[1][0] ||
				mat.m[1][1] != m[1][1] ||
				mat.m[1][2] != m[1][2] ||
				mat.m[1][3] != m[1][3])
			{
				return false;
			}
			if (mat.m[2][0] != m[2][0] ||
				mat.m[2][1] != m[2][1] ||
				mat.m[2][2] != m[2][2] ||
				mat.m[2][3] != m[2][3])
			{
				return false;
			}
			if (mat.m[3][0] != m[3][0] ||
				mat.m[3][1] != m[3][1] ||
				mat.m[3][2] != m[3][2] ||
				mat.m[3][3] != m[3][3])
			{
				return false;
			}
		}
		return true;
	}
	
	bool SMatrix44::operator!=(const SMatrix44& mat) const
	{
		return !(*this == mat);
	}

	SMatrix44 SMatrix44::operator/(float f ) const
	{

#ifdef WIN64
		//--------------------------
		float fInv = 1.0f / f;
		return SMatrix44(_11 * fInv, _12 * fInv, _13 * fInv, _14 * fInv,
			_21 * fInv, _22 * fInv, _23 * fInv, _24 * fInv,
			_31 * fInv, _32 * fInv, _33 * fInv, _34 * fInv,
			_41 * fInv, _42 * fInv, _43 * fInv, _44 * fInv);
		//--------------------------
#else

		float fInv = 1.0f / f;
		float fValue[16];
		__asm
		{
			mov  eax, this;
			movss xmm0, fInv;
			shufps xmm0, xmm0, 0;

			movups	xmm1, [eax];
			movups	xmm2, [eax + 16];
			movups	xmm3, [eax + 32];
			movups	xmm4, [eax + 48];

			mulps	xmm1, xmm0;
			mulps	xmm2, xmm0;
			mulps	xmm3, xmm0;
			mulps	xmm4, xmm0;

			movups[fValue], xmm1;
			movups[fValue + 16], xmm2;
			movups[fValue + 32], xmm3;
			movups[fValue + 48], xmm4;
		}
		return SMatrix44(fValue);
#endif
	}

	SMatrix44 SMatrix44::operator*(float f ) const
	{
#ifdef WIN64
		//--------------------------
		return SMatrix44(_11 * f, _12 * f, _13 * f, _14 * f,
			_21 * f, _22 * f, _23 * f, _24 * f,
			_31 * f, _32 * f, _33 * f, _34 * f,
			_41 * f, _42 * f, _43 * f, _44 * f);
		//--------------------------

#else
		float fValue[16];
		__asm
		{
			mov  eax, this;
			movss xmm0, f;
			shufps xmm0, xmm0, 0;

			movups xmm1, [eax];
			movups xmm2, [eax + 16];
			movups xmm3, [eax + 32];
			movups xmm4, [eax + 48];

			mulps	xmm1, xmm0;
			mulps	xmm2, xmm0;
			mulps	xmm3, xmm0;
			mulps	xmm4, xmm0;

			movups[fValue], xmm1;
			movups[fValue + 16], xmm2;
			movups[fValue + 32], xmm3;
			movups[fValue + 48], xmm4;
		}
		return SMatrix44(fValue);
#endif
	}

	SMatrix44 SMatrix44::operator-(const SMatrix44& mat) const
	{
#ifdef WIN64
		return SMatrix44(_11 - mat._11, _12 - mat._12, _13 - mat._13, _14 - mat._14,
			_21 - mat._21, _22 - mat._22, _23 - mat._23, _24 - mat._24,
			_31 - mat._31, _32 - mat._32, _33 - mat._33, _34 - mat._34,
			_41 - mat._41, _42 - mat._42, _43 - mat._43, _44 - mat._44);

#else

		float fValue[16];
		const float* pData = &mat.m[0][0];
		__asm
		{
			mov esi, this;
			mov edi, pData;

			movups xmm0, [esi];
			movups xmm1, [edi];
			subps  xmm0, xmm1;
			movups[fValue], xmm0;

			movups xmm0, [esi + 16];
			movups xmm1, [edi + 16];
			subps  xmm0, xmm1;
			movups[fValue + 16], xmm0;


			movups xmm0, [esi + 32];
			movups xmm1, [edi + 32];
			subps  xmm0, xmm1;
			movups[fValue + 32], xmm0;

			movups xmm0, [esi + 48];
			movups xmm1, [edi + 48];
			subps  xmm0, xmm1;
			movups[fValue + 48], xmm0;
		}
		return SMatrix44(fValue);
#endif
	}

	SMatrix44 SMatrix44::operator+(const SMatrix44& mat) const
	{
#ifdef WIN64
		return SMatrix44(_11 + mat._11, _12 + mat._12, _13 + mat._13, _14 + mat._14,
			_21 + mat._21, _22 + mat._22, _23 + mat._23, _24 + mat._24,
			_31 + mat._31, _32 + mat._32, _33 + mat._33, _34 + mat._34,
			_41 + mat._41, _42 + mat._42, _43 + mat._43, _44 + mat._44);

#else
		float fValue[16];
		const float* pData = &mat.m[0][0];
		__asm
		{
			mov esi, this;
			mov edi, pData;

			movups xmm0, [esi];
			movups xmm1, [edi];
			addps  xmm0, xmm1;
			movups[fValue], xmm0;

			movups xmm0, [esi + 16];
			movups xmm1, [edi + 16];
			addps  xmm0, xmm1;
			movups[fValue + 16], xmm0;


			movups xmm0, [esi + 32];
			movups xmm1, [edi + 32];
			addps  xmm0, xmm1;
			movups[fValue + 32], xmm0;


			movups xmm0, [esi + 48];
			movups xmm1, [edi + 48];
			addps  xmm0, xmm1;
			movups[fValue + 48], xmm0;
		}
		return SMatrix44(fValue);

#endif
	}

	SMatrix44 SMatrix44::operator*(const SMatrix44& mat) const
	{
#ifdef WIN64
		//--------------------------
		return SMatrix44(
			m[0][0] * mat.m[0][0] + m[0][1] * mat.m[1][0] + m[0][2] * mat.m[2][0] + m[0][3] * mat.m[3][0],
			m[0][0] * mat.m[0][1] + m[0][1] * mat.m[1][1] + m[0][2] * mat.m[2][1] + m[0][3] * mat.m[3][1],
			m[0][0] * mat.m[0][2] + m[0][1] * mat.m[1][2] + m[0][2] * mat.m[2][2] + m[0][3] * mat.m[3][2],
			m[0][0] * mat.m[0][3] + m[0][1] * mat.m[1][3] + m[0][2] * mat.m[2][3] + m[0][3] * mat.m[3][3],

			m[1][0] * mat.m[0][0] + m[1][1] * mat.m[1][0] + m[1][2] * mat.m[2][0] + m[1][3] * mat.m[3][0],
			m[1][0] * mat.m[0][1] + m[1][1] * mat.m[1][1] + m[1][2] * mat.m[2][1] + m[1][3] * mat.m[3][1],
			m[1][0] * mat.m[0][2] + m[1][1] * mat.m[1][2] + m[1][2] * mat.m[2][2] + m[1][3] * mat.m[3][2],
			m[1][0] * mat.m[0][3] + m[1][1] * mat.m[1][3] + m[1][2] * mat.m[2][3] + m[1][3] * mat.m[3][3],

			m[2][0] * mat.m[0][0] + m[2][1] * mat.m[1][0] + m[2][2] * mat.m[2][0] + m[2][3] * mat.m[3][0],
			m[2][0] * mat.m[0][1] + m[2][1] * mat.m[1][1] + m[2][2] * mat.m[2][1] + m[2][3] * mat.m[3][1],
			m[2][0] * mat.m[0][2] + m[2][1] * mat.m[1][2] + m[2][2] * mat.m[2][2] + m[2][3] * mat.m[3][2],
			m[2][0] * mat.m[0][3] + m[2][1] * mat.m[1][3] + m[2][2] * mat.m[2][3] + m[2][3] * mat.m[3][3],

			m[3][0] * mat.m[0][0] + m[3][1] * mat.m[1][0] + m[3][2] * mat.m[2][0] + m[3][3] * mat.m[3][0],
			m[3][0] * mat.m[0][1] + m[3][1] * mat.m[1][1] + m[3][2] * mat.m[2][1] + m[3][3] * mat.m[3][1],
			m[3][0] * mat.m[0][2] + m[3][1] * mat.m[1][2] + m[3][2] * mat.m[2][2] + m[3][3] * mat.m[3][2],
			m[3][0] * mat.m[0][3] + m[3][1] * mat.m[1][3] + m[3][2] * mat.m[2][3] + m[3][3] * mat.m[3][3]);
		//--------------------------
#else
		float fValue[16];
		const float* pData = &mat.m[0][0];
		__asm
		{
			mov esi, this;
			mov edi, pData;
			lea eax, fValue;
			
			movups xmm4, [edi];
			movups xmm5, [edi + 16];
			movups xmm6, [edi + 32];
			movups xmm7, [edi + 48];

			mov ecx, 4;   
			
		LOOPIT:  
			movss xmm0, [esi];
			shufps xmm0, xmm0, 0;
			mulps xmm0, xmm4;

			movss xmm1, [esi+4];
			shufps xmm1, xmm1, 0;
			mulps xmm1, xmm5;

			movss xmm2, [esi + 8];
			shufps xmm2, xmm2, 0;
			mulps xmm2, xmm6;

			movss xmm3, [esi + 12];
			shufps xmm3, xmm3, 0;
			mulps xmm3, xmm7;

			addps xmm0, xmm1;
			addps xmm0, xmm2;
			addps xmm0, xmm3;
			
			movups[eax], xmm0;

			add	esi, 16;
			add eax, 16;
		loop LOOPIT;
		}
		return SMatrix44(fValue);
#endif
	}

	SMatrix44 operator * (float f, const SMatrix44& mat)
	{
#ifdef WIN64
		//--------------------------
		return SMatrix44(f * mat._11, f * mat._12, f * mat._13, f * mat._14,
			f * mat._21, f * mat._22, f * mat._23, f * mat._24,
			f * mat._31, f * mat._32, f * mat._33, f * mat._34,
			f * mat._41, f * mat._42, f * mat._43, f * mat._44);
		//--------------------------
#else
		float fValue[16];
		const float* pData = &mat.m[0][0];
		__asm
		{
			mov  eax, pData;
			movss xmm0, f;
			shufps xmm0, xmm0, 0;

			movups xmm1, [eax];
			movups xmm2, [eax + 16];
			movups xmm3, [eax + 32];
			movups xmm4, [eax + 48];

			mulps	xmm1, xmm0;
			mulps	xmm2, xmm0;
			mulps	xmm3, xmm0;
			mulps	xmm4, xmm0;

			lea eax, fValue;
			movups[eax], xmm1;
			movups[eax + 16], xmm2;
			movups[eax + 32], xmm3;
			movups[eax + 48], xmm4;
		}
		return SMatrix44(fValue);
#endif
	}	
}