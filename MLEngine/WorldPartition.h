#pragma once
#include "Refcount.h"
namespace ML
{
	class MLENGINE_API CWorldPartition : public CRefcount
    {
    public:
        CWorldPartition();
        virtual ~CWorldPartition();
    };
}
