#include "VertexFormat.h"
namespace WL
{

	VertexFormat::VertexFormat()
	{
		for( auto item : mFlag )
		{
			item = 0;
		}
	}

	VertexFormat::VertexFormat(VertexFormatKey key, int nStream)
	{
		addElement(key, nStream);
	}

	void VertexFormat::addElement(VertexFormatKey key, INT32 nSteam /*= 0*/)
	{
		VertexElement element;
		
		switch (key)
		{
		case V_POSITION:
		{
			element.szName = "POSITION";
			element.nSize = sizeof(float) * 3;
			break;
		}
		case V_NORMAL:
		{
			mFlag[V_POSITION] = 1;
			element.szName = "NORMAL";
			element.nSize = sizeof(float) * 3;
			break;
		}
		case V_BINORMAL:
		{
			mFlag[V_POSITION] = 1;
			element.szName = "BINORMAL";
			element.nSize = sizeof(float) * 3;
			break;
		}
		case V_TANGETN:
		{
			mFlag[V_POSITION] = 1;
			element.szName = "TANGETN";
			element.nSize = sizeof(float) * 3;
			break;
		}
		case V_TEXTURE:
		{
			element.szName = "TEXTURE";
			element.nSize = sizeof(float) * 2;
			break;
		}
		case V_BONE_INDEX:
		{
			element.szName = "BONEINDEX";
			element.nSize = sizeof(UINT32) * 4;
			break;
		}
		case V_WEIGHT:
		{
			element.szName = "WEIGHT";
			element.nSize = sizeof(float) * 3;
			break;
		}
		case V_COLOR_FLOAT:
		{
			element.szName = "COLOR_FLOAT";
			element.nSize = sizeof(float) * 4;
			break;
		}
		case V_COLOR_UINT:
		{
			element.szName = "V_COLOR_UINT";
			element.nSize = sizeof(UINT32);
			break;
		}
		default:
			return;
		}
		element.key = key;
		element.nOffset = getVertexSize();
		element.nStreamIndex = nSteam;
		mFlag[key] = elementArray.size();
		elementArray.emplace_back(element);
		vertexSize += element.nSize;
	}


	INT32 VertexFormat::getVertexSize()
	{
		return vertexSize;
	}

	const VertexElement& VertexFormat::getVertexElement(int nIndex)
	{
		if (nIndex < (INT32)elementArray.size())
		{
			return elementArray[nIndex];
		}
		else
		{
			static  VertexElement gElement;
			gElement.szName = "";
			return gElement;
		}
	}

	int VertexFormat::getVerticesOffset() 
	{
		return getVertexMemberOffset(V_POSITION);
	}

	int VertexFormat::getNormalsOffset() 
	{
		return getVertexMemberOffset(V_NORMAL);
	}

	int VertexFormat::getBinormalsOffset()
	{
		return getVertexMemberOffset(V_BINORMAL);
	}

	int VertexFormat::getTangetOffset()
	{
		return getVertexMemberOffset(V_TANGETN);
	}

	int VertexFormat::getUVsOffset()
	{
		return getVertexMemberOffset(V_TEXTURE);
	}

	int VertexFormat::getBoneIndexOffset()
	{
		return getVertexMemberOffset(V_BONE_INDEX);
	}

	int VertexFormat::getWeightOffset()
	{
		return getVertexMemberOffset(V_WEIGHT);
	}

	int VertexFormat::getColorFloatOffset()
	{
		return getVertexMemberOffset(V_COLOR_FLOAT);
	}

	int VertexFormat::getColorUINTOffset()
	{
		return getVertexMemberOffset(V_COLOR_UINT);
	}

	int VertexFormat::getVertexMemberOffset(VertexFormatKey nKey)
	{
		if (nKey < V_MAX_COUNT)
		{
			int nIdx = mFlag[nKey];
			if (0 != nIdx)
			{
				if (nIdx < (int)elementArray.size())
				{
					return elementArray[nIdx].nOffset;
				}
			}
		}
		return -1;
	}

}