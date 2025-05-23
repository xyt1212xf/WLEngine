#pragma once
#include "Vector.h"
#include "Transform.h"
#include "ModelInstance.h"
#include "AABBox.h"
#include "Collision.h"
#include "ScriptEntity.h"
namespace WL
{
	struct SRenderUnitGrounp;
	class CBTNode;
	class WLENGINE_API CActorEntity : public CScriptEntity, public CTranformComponet
	{
		DeclareScriptClass(CActorEntity);
	public:
		CActorEntity();
		virtual ~CActorEntity();
		virtual void setPosition(const Vec3F& pos);
		virtual void setScale(const Vec3F& scale);
		virtual void setScale(float fScale);
		virtual void setRotation(const Vec3F& rotation);
		
		virtual void addModel(CModelInstance* pModelInstance);
		virtual void addModel(CModel* pModel);
		virtual void addModel(CModel* pModel, INT32 nSlot);
		virtual void addModel(const std::string& szModel, INT32 nSlot);
		virtual void removeModel(CModel* pModel);
		virtual void removeModel(INT32 nPart);
		virtual void draw(UINT32 nTime);
		void addComponentByScript(CScriptEntity* pComponent);
		void removeComponentByScript(CScriptEntity* pComponent);
		bool hasComponetByScript(const std::string& componentName);

		void test(int value);
		

		void setViewMT(SMatrix44* pMT);
		void setTransposeViewMT(SMatrix44* pMT);
		void setProjectMT(SMatrix44* pMT);
		void setTransposeProjectMT(SMatrix44* pMT);
		void addBehaviorTree(const std::string& szComponent);
		const std::map<INT32, CModelInstance*>& getModels() const;
		CModelInstance* getModelInstance(int nKey = 0) const;
		

	private:
		virtual void _tick(UINT32 dt) override;

	protected:
		std::map<INT32, CModelInstance*> mModelInstanceMap;
		std::forward_list<CBTNode*>	mBTNodes;

		int mTest = 0;
	};
}