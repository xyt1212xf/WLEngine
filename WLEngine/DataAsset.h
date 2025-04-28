#pragma once
#include "Export.h"
#include "Refcount.h"
#include "ScriptEntity.h"

namespace WL
{
	struct WLENGINE_API DataAsset : public CScriptEntity, public CRefcount
	{
		DataAsset()
		{

		}
	};
}

