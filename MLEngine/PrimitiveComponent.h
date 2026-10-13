#pragma once
#include "SceneComponent.h"
namespace ML
{
	class MLENGINE_API CPrimitiveComponent : public CSceneComponent
    {
    public:
        CPrimitiveComponent();
        virtual ~CPrimitiveComponent();
        void SetVisible(bool bInVisible) { bVisible = bInVisible; }
        bool IsVisible() const { return bVisible; }

    protected:
    	bool bVisible = false;
    };
}
