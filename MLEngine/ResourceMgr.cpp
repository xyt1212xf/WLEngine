#include <fstream>
#include "ResourceMgr.h"
#include "MLEngine.h"
#include "jsonHeader.h"

namespace ML
{
	struct SResourceFileHeader
	{
		unsigned int magic_number;
		int version;
		int nobjects;
	};

	struct SFaceType
	{
		int vIndex1, vIndex2, vIndex3;
		int tIndex1, tIndex2, tIndex3;
		int nIndex1, nIndex2, nIndex3;
	};

	CResourceMgr::CResourceMgr()
	{
	}

	CResourceMgr::~CResourceMgr()
	{

	}


	bool CResourceMgr::initialize()
	{
		return true;
	}

	void CResourceMgr::unInitialize()
	{

	}

	void CResourceMgr::releaseResource()
	{

	}

}