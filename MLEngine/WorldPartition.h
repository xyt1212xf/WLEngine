#pragma once
#include "Refcount.h"
namespace ML
{
	class CWorldPartition : public CRefcount
    {
    public:
        CWorldPartition();
        virtual ~CWorldPartition();
    };
}