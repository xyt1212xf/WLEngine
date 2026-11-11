#pragma once
#include "Actor.h"
namespace ML
{
	class CLandscapeComponent;
	class CMaterial;
	class CLandscapeProxy : public CActor
	{
	public:
		CLandscapeProxy(const std::string& Name);
		CLandscapeProxy(std::string&& Name) noexcept;

		void CreateLandscape(int32 NumX, int32 NumY, int32 SizeQuads, CMaterial* Mat);
		CLandscapeComponent* GetComponent(int32 X, int32 Y) const;

	protected:
		std::vector<CLandscapeComponent*> Tiles;

	private:
		int32 mNumX = 0;   // 每行 tile 数（CreateLandscape 的 NumX）
		int32 mNumY = 0;   // 每列 tile 数（CreateLandscape 的 NumY）
	};


	class CLandscape : public CLandscapeProxy
	{
	public:
		CLandscape(const std::string& Name);
	};
}


