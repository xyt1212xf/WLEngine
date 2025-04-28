#pragma once

namespace WL
{
#pragma pack( push , 1 )
	struct CMemoryCookie
	{
		size_t				m_stSize;
		union
		{
			//CMemCallStackInfo* m_pStackInfo;
			size_t				m_uCheckSum;
		};
	};
#pragma pack( pop )
}
