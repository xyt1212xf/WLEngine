#pragma once
namespace ML
{
	class CLightSceneProxy;
	class CLightSceneInfo
	{
		friend class CRenderer;
	public:
		CLightSceneProxy* Proxy = nullptr;
	};
}
