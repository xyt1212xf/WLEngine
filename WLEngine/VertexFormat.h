#pragma once
#include "Common.h"
#include "Vector.h"
namespace WL
{
	enum VertexFormatKey
	{
		V_POSITION,
		V_NORMAL,
		V_BINORMAL,
		V_TANGETN,
		V_TEXTURE,
		V_BONE_INDEX,
		V_WEIGHT,
		V_COLOR_FLOAT,
		V_COLOR_UINT,
		V_MAX_COUNT,
	};

	struct VertexElement
	{
		VertexFormatKey key;
		std::string szName;
		INT32 nStreamIndex;
		INT16 nSize;
		INT16 nOffset;
	};

	struct VertexFormat
	{
		//////////////////////////////////////////////////////////////////////////
		VertexFormat();
		VertexFormat(VertexFormatKey key, int nStream);
		void addElement(VertexFormatKey key, INT32 nSteam = 0);
		INT32 getVertexSize();
		const VertexElement& getVertexElement(int nIndex); 
		int getVerticesOffset();
		int getNormalsOffset();
		int getBinormalsOffset();
		int getTangetOffset();
		int getUVsOffset();
		int getBoneIndexOffset();
		int getWeightOffset();
		int getColorFloatOffset();
		int getColorUINTOffset();
		int getVertexMemberOffset(VertexFormatKey nKey);
		//////////////////////////////////////////////////////////////////////////
		std::vector<VertexElement> elementArray;
		INT32 vertexSize = 0;
		std::array<INT32, VertexFormatKey::V_MAX_COUNT> mFlag;
	};

}