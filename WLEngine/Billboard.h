#pragma once
#include "ScriptEntity.h"
#include "Transform.h"
namespace WL
{
	class CBillboard : public CScriptEntity,
						public CTranformComponet
	{
		DeclareScriptClass(CBillboard);
	public:
		CBillboard();
		virtual ~CBillboard();
		bool loadResource(const std::string& szFile);

	private:

	};
}