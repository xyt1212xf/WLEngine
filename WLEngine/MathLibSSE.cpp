#include "MathLibSSE.h"
namespace WL
{
	namespace Math
	{
#ifndef WIN64
		float vec3DotSSE(const Vec3F* pV1, const Vec3F* pV2)
		{
			float f[4];
			__asm
			{
				mov esi, pV1;
				mov edi, pV2;
				movups xmm0, [esi];
				movups xmm1, [edi];

				mulps xmm0, xmm1;
				movups  f, xmm0;
			}
			return f[0] + f[1] + f[2];
		}

		void vec3CrossSSE(InOut Vec3F* pOut, const Vec3F* pV1, const Vec3F* pV2)
		{
			float f[4];
			__asm
			{
				mov esi, pV1;
				mov edi, pV2;

				movups xmm0, [esi];
				movups xmm1, [edi];

				movaps xmm2, xmm0;
				movaps xmm3, xmm1;

				shufps xmm0, xmm0, 0xc9;
				shufps xmm1, xmm1, 0xd2;
				mulps xmm0, xmm1;

				shufps xmm2, xmm2, 0xd2;
				shufps xmm3, xmm3, 0xc9;
				mulps xmm2, xmm3;

				subps xmm0, xmm2;
				movups[f], xmm0;
			}
			pOut->x = f[0];
			pOut->y = f[1];
			pOut->z = f[2];	
		}


		Vec4F matrixMulVec4SSE(In const Matrix44& matrix, In const Vec4F& v)
		{
			float pArray[4];
			__asm
			{
				mov esi, matrix;
				mov edi, v;

				movups  xmm0, [edi];
				movups  xmm1, [esi];

				shufps  xmm0, xmm0, 0;
				mulps	xmm1, xmm0;

				movss	xmm0, [edi + 4];
				shufps  xmm0, xmm0, 0;
				movups  xmm2, [esi + 16];
				mulps	xmm2, xmm0;

				movss	xmm0, [edi + 8];
				shufps  xmm0, xmm0, 0;
				movups  xmm3, [esi + 32];
				mulps	xmm3, xmm0;

				movss	xmm0, [edi + 12];
				shufps  xmm0, xmm0, 0;
				movups  xmm4, [esi + 48];
				mulps	xmm4, xmm0;

				addps xmm1, xmm2;
				addps xmm1, xmm3;
				addps xmm1, xmm4;

				movups[pArray], xmm1;
			}
			return	Vec4F(pArray);
		}


		Vec3F matrixMulVec3SSE(In const Matrix44& matrix, In const Vec3F& v)
		{
			float pArray[4];
			__asm
			{
				mov esi, matrix;
				mov edi, v;

				movups  xmm0, [edi];
				movups  xmm1, [esi];

				shufps  xmm0, xmm0, 0;
				mulps	xmm1, xmm0;

				movss	xmm0, [edi + 4];
				shufps  xmm0, xmm0, 0;
				movups  xmm2, [esi + 16];
				mulps	xmm2, xmm0;

				movss	xmm0, [edi + 8];
				shufps  xmm0, xmm0, 0;
				movups  xmm3, [esi + 32];
				mulps	xmm3, xmm0;

				movups xmm0, [esi + 48];

				addps xmm1, xmm0;
				addps xmm1, xmm2;
				addps xmm1, xmm3;
				
				movups[pArray], xmm1;
			}
			return Vec3F(pArray);
		}

		bool matrixInverseSSE(InOut Matrix44* pOut, IN const Matrix44* pM)
		{
			/*
			float det = A * B
						C * D
			*/	
#if 0
			float det = (pM->m[0][0] * pM->m[1][1] - pM->m[0][1] * pM->m[1][0]) * (pM->m[2][2] * pM->m[3][3] - pM->m[2][3] * pM->m[3][2]) -
						(pM->m[0][0] * pM->m[1][2] - pM->m[0][2] * pM->m[1][0]) * (pM->m[2][1] * pM->m[3][3] - pM->m[2][3] * pM->m[3][1]) +
						(pM->m[0][0] * pM->m[1][3] - pM->m[0][3] * pM->m[1][0]) * (pM->m[2][1] * pM->m[3][2] - pM->m[2][2] * pM->m[3][1]) +
						(pM->m[0][1] * pM->m[1][2] - pM->m[0][2] * pM->m[1][1]) * (pM->m[2][0] * pM->m[3][3] - pM->m[2][3] * pM->m[3][0]) -
						(pM->m[0][1] * pM->m[1][3] - pM->m[0][3] * pM->m[1][1]) * (pM->m[2][0] * pM->m[3][2] - pM->m[2][2] * pM->m[3][0]) +
						(pM->m[0][2] * pM->m[1][3] - pM->m[0][3] * pM->m[1][2]) * (pM->m[2][0] * pM->m[3][1] - pM->m[2][1] * pM->m[3][0]);
#else
			float pA[4];
			float pB[4];
			__asm
			{
				mov esi, pM;

				//A
				movups  xmm0, [pM];
				shufps  xmm0, xmm0, 0;
				movups  xmm1, [esi + 16];
				shufps  xmm1, xmm1, 111001b;
				mulps	xmm0, xmm1;

				movups  xmm1, [esi];
				shufps  xmm1, xmm1, 111001b;
				movups  xmm2, [esi + 16];
				shufps  xmm2, xmm2, 0;
				mulps	xmm1, xmm2;
				subps	xmm0, xmm1;


				//B
				movups  xmm1, [esi + 32];
				shufps  xmm1, xmm1, 010110b;
				movups  xmm2, [esi + 48];
				shufps  xmm2, xmm2, 101111b;
				mulps	xmm1, xmm2;

				movups  xmm2, [esi + 32];
				shufps  xmm2, xmm2, 101111b;
				movups  xmm3, [esi + 48];
				shufps  xmm3, xmm3, 010110b;
				mulps	xmm2, xmm3;
				subps	xmm1, xmm2;

				//C
				movups  xmm2, [esi];
				shufps  xmm2, xmm2, 100101b;
				movups  xmm3, [esi + 16];
				shufps  xmm3, xmm3, 111110b;
				mulps	xmm2, xmm3;

				movups  xmm3, [esi];
				shufps  xmm3, xmm3, 111110b;
				movups  xmm4, [esi + 16];
				shufps  xmm4, xmm4, 100101b;
				mulps	xmm3, xmm4;
				subps	xmm2, xmm3;

				//D
				movups  xmm3, [esi + 32];
				shufps  xmm3, xmm3, 0b;
				movups  xmm4, [esi + 48];
				shufps  xmm4, xmm4, 11011b;
				mulps	xmm3, xmm4;

				movups  xmm4, [esi + 32];
				shufps  xmm4, xmm4, 11011b;
				movups  xmm5, [esi + 48];
				shufps  xmm5, xmm5, 0b;
				mulps	xmm4, xmm5;
				subps	xmm3, xmm4;

				mulps xmm0, xmm1;
				mulps xmm2, xmm3;

				movups  pA, xmm0;
				movups  pB, xmm2;
			}
			//float det =	(pA[0]) -
			//			(pA[1]) +
			//			(pA[2]) +
			//			(pB[0]) -
			//			(pB[1]) +
			//			(pB[2]);
			float det = 0;
			if (fabs(det) <= ROUNDING_ERROR_f32)
			{
				return false;
			}
#endif

			det = 1 / det;

			pOut->m[0][0] = det * (pM->m[1][1] * (pM->m[2][2] * pM->m[3][3] - pM->m[2][3] * pM->m[3][2]) +
								   pM->m[1][2] * (pM->m[2][3] * pM->m[3][1] - pM->m[2][1] * pM->m[3][3]) +
								   pM->m[1][3] * (pM->m[2][1] * pM->m[3][2] - pM->m[2][2] * pM->m[3][1]));

			pOut->m[0][1] = det * (pM->m[2][1] * (pM->m[0][2] * pM->m[3][3] - pM->m[0][3] * pM->m[3][2]) +
								   pM->m[2][2] * (pM->m[0][3] * pM->m[3][1] - pM->m[0][1] * pM->m[3][3]) +
								   pM->m[2][3] * (pM->m[0][1] * pM->m[3][2] - pM->m[0][2] * pM->m[3][1]));

			pOut->m[0][2] = det * (pM->m[3][1] * (pM->m[0][2] * pM->m[1][3] - pM->m[0][3] * pM->m[1][2]) +
				pM->m[3][2] * (pM->m[0][3] * pM->m[1][1] - pM->m[0][1] * pM->m[1][3]) +
				pM->m[3][3] * (pM->m[0][1] * pM->m[1][2] - pM->m[0][2] * pM->m[1][1]));

			pOut->m[0][3] = det * (pM->m[0][1] * (pM->m[1][3] * pM->m[2][2] - pM->m[1][2] * pM->m[2][3]) +
				pM->m[0][2] * (pM->m[1][1] * pM->m[2][3] - pM->m[1][3] * pM->m[2][1]) +
				pM->m[0][3] * (pM->m[1][2] * pM->m[2][1] - pM->m[1][1] * pM->m[2][2]));

			pOut->m[1][0] = det * (pM->m[1][2] * (pM->m[2][0] * pM->m[3][3] - pM->m[2][3] * pM->m[3][0]) +
				pM->m[1][3] * (pM->m[2][2] * pM->m[3][0] - pM->m[2][0] * pM->m[3][2]) +
				pM->m[1][0] * (pM->m[2][3] * pM->m[3][2] - pM->m[2][2] * pM->m[3][3]));

			pOut->m[1][1] = det * (pM->m[2][2] * (pM->m[0][0] * pM->m[3][3] - pM->m[0][3] * pM->m[3][0]) +
				pM->m[2][3] * (pM->m[0][2] * pM->m[3][0] - pM->m[0][0] * pM->m[3][2]) +
				pM->m[2][0] * (pM->m[0][3] * pM->m[3][2] - pM->m[0][2] * pM->m[3][3]));

			pOut->m[1][2] = det * (pM->m[3][2] * (pM->m[0][0] * pM->m[1][3] - pM->m[0][3] * pM->m[1][0]) +
				pM->m[3][3] * (pM->m[0][2] * pM->m[1][0] - pM->m[0][0] * pM->m[1][2]) +
				pM->m[3][0] * (pM->m[0][3] * pM->m[1][2] - pM->m[0][2] * pM->m[1][3]));

			pOut->m[1][3] = det * (pM->m[0][2] * (pM->m[1][3] * pM->m[2][0] - pM->m[1][0] * pM->m[2][3]) +
				pM->m[0][3] * (pM->m[1][0] * pM->m[2][2] - pM->m[1][2] * pM->m[2][0]) +
				pM->m[0][0] * (pM->m[1][2] * pM->m[2][3] - pM->m[1][3] * pM->m[2][2]));

			pOut->m[2][0] = det * (pM->m[1][3] * (pM->m[2][0] * pM->m[3][1] - pM->m[2][1] * pM->m[3][0]) +
				pM->m[1][0] * (pM->m[2][1] * pM->m[3][3] - pM->m[2][3] * pM->m[3][1]) +
				pM->m[1][1] * (pM->m[2][3] * pM->m[3][0] - pM->m[2][0] * pM->m[3][3]));

			pOut->m[2][1] = det * (pM->m[2][3] * (pM->m[0][0] * pM->m[3][1] - pM->m[0][1] * pM->m[3][0]) +
				pM->m[2][0] * (pM->m[0][1] * pM->m[3][3] - pM->m[0][3] * pM->m[3][1]) +
				pM->m[2][1] * (pM->m[0][3] * pM->m[3][0] - pM->m[0][0] * pM->m[3][3]));

			pOut->m[2][2] = det * (pM->m[3][3] * (pM->m[0][0] * pM->m[1][1] - pM->m[0][1] * pM->m[1][0]) +
				pM->m[3][0] * (pM->m[0][1] * pM->m[1][3] - pM->m[0][3] * pM->m[1][1]) +
				pM->m[3][1] * (pM->m[0][3] * pM->m[1][0] - pM->m[0][0] * pM->m[1][3]));

			pOut->m[2][3] = det * (pM->m[0][3] * (pM->m[1][1] * pM->m[2][0] - pM->m[1][0] * pM->m[2][1]) +
				pM->m[0][0] * (pM->m[1][3] * pM->m[2][1] - pM->m[1][1] * pM->m[2][3]) +
				pM->m[0][1] * (pM->m[1][0] * pM->m[2][3] - pM->m[1][3] * pM->m[2][0]));

			pOut->m[3][0] = det * (pM->m[1][0] * (pM->m[2][2] * pM->m[3][1] - pM->m[2][1] * pM->m[3][2]) +
				pM->m[1][1] * (pM->m[2][0] * pM->m[3][2] - pM->m[2][2] * pM->m[3][0]) +
				pM->m[1][2] * (pM->m[2][1] * pM->m[3][0] - pM->m[2][0] * pM->m[3][1]));

			pOut->m[3][1] = det * (pM->m[2][0] * (pM->m[0][2] * pM->m[3][1] - pM->m[0][1] * pM->m[3][2]) +
				pM->m[2][1] * (pM->m[0][0] * pM->m[3][2] - pM->m[0][2] * pM->m[3][0]) +
				pM->m[2][2] * (pM->m[0][1] * pM->m[3][0] - pM->m[0][0] * pM->m[3][1]));

			pOut->m[3][2] = det * (pM->m[3][0] * (pM->m[0][2] * pM->m[1][1] - pM->m[0][1] * pM->m[1][2]) +
				pM->m[3][1] * (pM->m[0][0] * pM->m[1][2] - pM->m[0][2] * pM->m[1][0]) +
				pM->m[3][2] * (pM->m[0][1] * pM->m[1][0] - pM->m[0][0] * pM->m[1][1]));

			pOut->m[3][3] = det * (pM->m[0][0] * (pM->m[1][1] * pM->m[2][2] - pM->m[1][2] * pM->m[2][1]) +
				pM->m[0][1] * (pM->m[1][2] * pM->m[2][0] - pM->m[1][0] * pM->m[2][2]) +
				pM->m[0][2] * (pM->m[1][0] * pM->m[2][1] - pM->m[1][1] * pM->m[2][0]));
			return true;
		}
#endif
	}
}