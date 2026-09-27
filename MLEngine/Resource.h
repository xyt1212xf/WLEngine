#pragma once
#include "Common.h"
#include "Refcount.h"
namespace ML
{
	enum ResourceType
	{
		Mesh,
		Texture,
		Shader,
		Material,
		Wnd,
		Font,
		Model,
		SurfaceView,
		Resource,
		UnKnow = 0XFFFF,
	};
	
	enum class ResourceStatus : uint8
	{
		loading,
		loaded,
		unloading,
		unloaded,
		UnKnow = 0XFFFF,
	};

	//const static std::string szTextureFlag[] =
	//{
	//	"baseTexture",
	//	"normalTexture",
	//	"detailTexture",
	//	"lightMapTexture",
	//	"textureArray",
	//};


	class CResource : public CRefcount
	{
		friend class CResourceMgr;
		friend class CResourceFactory;
		friend class CUIFactory;

	public:
		CResource();
		virtual ~CResource();
		UINT32 getUUID();

		void setResourceName(const std::string& szResourceName);
		const std::string& getResourceName()const;

		void setName(const std::string& szName);
		const std::string& getName()const;
		
		ResourceStatus GetStatus() const;
		ResourceType getType();
		std::string getTypeName() const;
	
		virtual bool initialise();

	private:
		void SetStatus(ResourceStatus status);

	protected:
		ResourceStatus mStatus = ResourceStatus::UnKnow;
		ResourceType sResType = UnKnow;
		ResourceType mType = UnKnow;
		UINT32	mUUID = 0;
		std::string mName = "";
		std::string mResourceName = "";
		std::string mTypeName = "";
	};
}