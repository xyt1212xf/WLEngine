#include "Level.h"
#include "WorldSet.h"
#include "Actor.h"
#include "Component.h"
#include "SceneComponent.h"
#include "MeshComponent.h"
#include "StaticMesh.h"
#include "StaticMeshComponent.h"
#include "MaterialMgr.h"
#include "Material.h"
#include "TextureMgr.h"
#include "Texture2D.h"
#include "MathLib.h"
#include "MLEngine.h"
#include "lua.hpp"
#include "Vector.h"
namespace ML
{
	// ---------------------------------------------------------------------------
	// Level (*.me) deserialization helpers.
	//
	// UE counterpart: a level package is deserialized through FArchive into
	// ULevel + AActor + UActorComponent. Here the level is authored as a Lua table
	// literal, so the "archive" is a throwaway lua_State. It is created and
	// destroyed entirely inside CLevel::Load(), which runs on a worker thread -
	// so it never touches CScriptPlug's gameplay VM and needs no locking.
	//
	// The state is sandboxed: only base / table / math / string are opened, so a
	// level file cannot reach io, os, load, dofile or require.
	// ---------------------------------------------------------------------------
	namespace
	{
		void OpenSandboxLibs(lua_State* L)
		{
			static const luaL_Reg SandboxLibs[] =
			{
				{ "",              luaopen_base   },
				{ LUA_TABLIBNAME,  luaopen_table  },
				{ LUA_MATHLIBNAME, luaopen_math   },
				{ LUA_STRLIBNAME,  luaopen_string },
				{ nullptr, nullptr }
			};

			for (const luaL_Reg* Lib = SandboxLibs; nullptr != Lib->func; ++Lib)
			{
				luaL_requiref(L, Lib->name, Lib->func, 1);
				lua_pop(L, 1);
			}
		}

		// Pushes t[Key]; returns false (and pushes nothing) when the field is absent.
		bool TryGetField(lua_State* L, int TableIdx, const char* Key)
		{
			const int Abs = lua_absindex(L, TableIdx);
			lua_pushstring(L, Key);
			lua_rawget(L, Abs);
			if (lua_isnil(L, -1))
			{
				lua_pop(L, 1);
				return false;
			}
			return true;
		}

		// Reads a 3-component numeric array table { x, y, z }.
		bool ReadVec3(lua_State* L, int Idx, Vec3F& out)
		{
			if (!lua_istable(L, Idx))
			{
				return false;
			}
			const int Abs = lua_absindex(L, Idx);
			lua_rawgeti(L, Abs, 1);
			out.x = (float)lua_tonumber(L, -1);
			lua_pop(L, 1);
			lua_rawgeti(L, Abs, 2);
			out.y = (float)lua_tonumber(L, -1);
			lua_pop(L, 1);
			lua_rawgeti(L, Abs, 3);
			out.z = (float)lua_tonumber(L, -1);
			lua_pop(L, 1);
			return true;
		}

		// UE: FTransform. Rotation is authored as Euler DEGREES (editor-style, like
		// FRotator) and stored as a quaternion (like FTransform::Rotation).
		// Missing fields fall back to identity location / rotation / unit scale.
		bool ReadTransform(lua_State* L, int Idx, Vec3F& OutLocation, CQuaternion& OutRotation, Vec3F& OutScale)
		{
			OutLocation = Vec3F(0.0f, 0.0f, 0.0f);
			OutScale    = Vec3F(1.0f, 1.0f, 1.0f);
			OutRotation = CQuaternion(0.0f, 0.0f, 0.0f, 1.0f);

			if (TryGetField(L, Idx, "Location"))
			{
				ReadVec3(L, -1, OutLocation);
				lua_pop(L, 1);
			}
			if (TryGetField(L, Idx, "Scale"))
			{
				ReadVec3(L, -1, OutScale);
				lua_pop(L, 1);
			}
			if (TryGetField(L, Idx, "Rotation"))
			{
				Vec3F EulerDeg(0.0f, 0.0f, 0.0f);
				if (ReadVec3(L, -1, EulerDeg))
				{
					CQuaternion Q;
					Q.set(Math::angleToRadian(EulerDeg.x),
						  Math::angleToRadian(EulerDeg.y),
						  Math::angleToRadian(EulerDeg.z));
					OutRotation = Q;
				}
				lua_pop(L, 1);
			}
			return true;
		}

		// UE counterpart: a level references a static mesh ASSET by path.
		// The "Procedural" branch is a P0 stand-in that builds the mesh from a
		// CStaticMeshDescription, until the .ma asset importer exists.
		CStaticMesh* LoadMesh(lua_State* L, int MeshIdx)
		{
			std::string Source = "";
			if (TryGetField(L, MeshIdx, "Source"))
			{
				Source = lua_tostring(L, -1);
				lua_pop(L, 1);
			}

			if ("Asset" == Source)
			{
				std::string Path;
				if (TryGetField(L, MeshIdx, "Path"))
				{
					Path = lua_tostring(L, -1);
					lua_pop(L, 1);
				}
				// TODO(P1): resolve through the asset registry + a mesh cache once
				// the .ma importer exists. Until then the component gets no mesh.
				OutputDebugStringA(("CLevel: mesh asset loading not implemented yet: " + Path + "\n").c_str());
				return nullptr;
			}

			if ("Procedural" == Source)
			{
				std::string Shape;
				if (TryGetField(L, MeshIdx, "Shape"))
				{
					Shape = lua_tostring(L, -1);
					lua_pop(L, 1);
				}
				if ("Cube" != Shape)
				{
					OutputDebugStringA(("CLevel: unsupported procedural shape: " + Shape + "\n").c_str());
					return nullptr;
				}

				Vec3F Center(0.0f, 0.0f, 0.0f);
				Vec3F HalfExtents(1.0f, 1.0f, 1.0f);
				if (TryGetField(L, MeshIdx, "Center"))
				{
					ReadVec3(L, -1, Center);
					lua_pop(L, 1);
				}
				if (TryGetField(L, MeshIdx, "HalfExtents"))
				{
					ReadVec3(L, -1, HalfExtents);
					lua_pop(L, 1);
				}

				CStaticMesh* Mesh = new CStaticMesh();
				Mesh->AddMaterialSlot("Default");
				Mesh->GetSourceModel(Mesh->AddSourceModel())->CreateCube(Center, HalfExtents);
				Mesh->Build();
				return Mesh;
			}

			return nullptr;
		}

		// UE counterpart: the level's REFERENCES to material assets (the import
		// table of a map package). A real UE material is an asset with its own
		// file, resolved through the asset registry; until .ma material assets
		// exist the level declares them inline, so the level also owns them.
		//
		// Each entry names a shader pair (a P0 stand-in for the material's
		// FShaderMap / shader permutation) and at most one base-color texture.
		void ParseMaterials(lua_State* L, int MaterialsIdx,
			std::unordered_map<std::string, CMaterial*>& OutMaterials)
		{
			const int AbsIdx = lua_absindex(L, MaterialsIdx);
			const int NumMaterials = (int)lua_rawlen(L, AbsIdx);
			for (int i = 1; i <= NumMaterials; ++i)
			{
				lua_rawgeti(L, AbsIdx, i);
				const int EntryIdx = lua_absindex(L, -1);

				std::string Name;
				std::string VSName;
				std::string PSName;
				std::string TexturePath;

				if (TryGetField(L, EntryIdx, "Name"))    { Name = lua_tostring(L, -1);     lua_pop(L, 1); }
				if (TryGetField(L, EntryIdx, "VS"))      { VSName = lua_tostring(L, -1);   lua_pop(L, 1); }
				if (TryGetField(L, EntryIdx, "PS"))      { PSName = lua_tostring(L, -1);   lua_pop(L, 1); }
				if (TryGetField(L, EntryIdx, "Texture")) { TexturePath = lua_tostring(L, -1); lua_pop(L, 1); }

				if (Name.empty() || VSName.empty() || PSName.empty())
				{
					OutputDebugStringA("CLevel: material entry needs Name / VS / PS\n");
					lua_pop(L, 1);
					continue;
				}

				CMaterial* Material = new CMaterial();
				Material->SetName(Name);
				Material->SetShaders(VSName, PSName);

				if (!TexturePath.empty())
				{
					// Textures are shared assets owned by CTextureMgr, not by the level.
					CTextureMgr* TexMgr = GEngine ? GEngine->GetMgr<CTextureMgr>() : nullptr;
					CTexture2D* Texture = TexMgr ? TexMgr->Load(TexturePath) : nullptr;
					if (nullptr == Texture)
					{
						OutputDebugStringA(("CLevel: failed to load texture \"" + TexturePath
							+ "\" for material \"" + Name + "\"\n").c_str());
					}
					// Enable it even when the load failed: it tells the renderer this
					// material's shaders sample t0, so the draw is skipped instead of
					// reading an undefined descriptor.
					Material->EnableBaseColorTexture(true);
					Material->SetBaseColorTexture(Texture);
				}

				auto It = OutMaterials.find(Name);
				if (It != OutMaterials.end())
				{
					SafeDelete(It->second);   // duplicate declaration: last one wins
				}
				OutMaterials[Name] = Material;
				lua_pop(L, 1);
			}
		}

		// Creates one component from its level entry and attaches it to Owner.
		// UE counterpart: NewObject<UStaticMeshComponent>(Actor), driven by the
		// component class named in the level.
		//
		// InLevelMaterials: materials this same level declared inline. Checked before
		// CMaterialMgr so a level-local material wins over an engine one of the same name.
		CComponent* ParseComponent(lua_State* L, int CompIdx, CActor* Owner,
			const std::unordered_map<std::string, CMaterial*>& InLevelMaterials)
		{
			std::string Type;
			if (TryGetField(L, CompIdx, "Type"))
			{
				Type = lua_tostring(L, -1);
				lua_pop(L, 1);
			}

			if ("StaticMesh" == Type)
			{
				CStaticMeshComponent* MeshComp = Owner->AddComponent<CStaticMeshComponent>();

				// UE: USceneComponent::bVisible
				if (TryGetField(L, CompIdx, "Visible"))
				{
					MeshComp->SetVisible(0 != lua_toboolean(L, -1));
					lua_pop(L, 1);
				}

				if (TryGetField(L, CompIdx, "Mesh"))
				{
					const int MeshIdx = lua_absindex(L, -1);
					MeshComp->SetStaticMesh(LoadMesh(L, MeshIdx));
					lua_pop(L, 1);
				}

				// UE: UMeshComponent::OverrideMaterials - resolved by material name here.
				if (TryGetField(L, CompIdx, "OverrideMaterials"))
				{
					const int MatIdx = lua_absindex(L, -1);
					const int NumMats = (int)lua_rawlen(L, MatIdx);
					CMaterialMgr* MatMgr = GEngine ? GEngine->GetMgr<CMaterialMgr>() : nullptr;
					for (int m = 1; m <= NumMats; ++m)
					{
						lua_rawgeti(L, MatIdx, m);
						const std::string MatName = lua_tostring(L, -1);
						lua_pop(L, 1);

						CMaterial* Mat = nullptr;
						auto It = InLevelMaterials.find(MatName);
						if (It != InLevelMaterials.end())
						{
							Mat = It->second;
						}
						else if (nullptr != MatMgr)
						{
							Mat = MatMgr->Find(MatName);   // engine-provided material
						}
						if (nullptr == Mat)
						{
							OutputDebugStringA(("CLevel: unknown material \"" + MatName
								+ "\"; falling back to the mesh's own material slot\n").c_str());
						}
						MeshComp->SetOverrideMaterial(m - 1, Mat);
					}
					lua_pop(L, 1);
				}

				return MeshComp;
			}

			OutputDebugStringA(("CLevel: unsupported component type: " + Type + "\n").c_str());
			return nullptr;
		}

		// UE counterpart: ULevel::Serialize reading one AActor off the archive.
		CActor* ParseActor(lua_State* L, int ActorIdx,
			const std::unordered_map<std::string, CMaterial*>& InLevelMaterials)
		{
			std::string ActorName;
			if (TryGetField(L, ActorIdx, "Name"))
			{
				ActorName = lua_tostring(L, -1);
				lua_pop(L, 1);
			}

			CActor* Actor = new CActor(ActorName);

			if (TryGetField(L, ActorIdx, "Label"))
			{
				Actor->SetActorLabel(lua_tostring(L, -1));
				lua_pop(L, 1);
			}
			if (TryGetField(L, ActorIdx, "Script"))
			{
				Actor->SetLuaClass(lua_tostring(L, -1));
				lua_pop(L, 1);
			}
			if (TryGetField(L, ActorIdx, "Tick"))
			{
				Actor->SetActorTickEnabled(0 != lua_toboolean(L, -1));
				lua_pop(L, 1);
			}

			// UE: AActor::Tags
			if (TryGetField(L, ActorIdx, "Tags"))
			{
				const int TagsIdx = lua_absindex(L, -1);
				const int NumTags = (int)lua_rawlen(L, TagsIdx);
				for (int t = 1; t <= NumTags; ++t)
				{
					lua_rawgeti(L, TagsIdx, t);
					Actor->AddTag(lua_tostring(L, -1));
					lua_pop(L, 1);
				}
				lua_pop(L, 1);
			}

			// UE: AActor::SetActorTransform()
			Vec3F Loc;
			CQuaternion Rot;
			Vec3F Scl;
			if (TryGetField(L, ActorIdx, "Transform"))
			{
				ReadTransform(L, -1, Loc, Rot, Scl);
				lua_pop(L, 1);
			}
			Actor->SetActorTransform(Loc, Rot, Scl);

			// Components. The root CSceneComponent already exists (created by the
			// CActor ctor), so these are additional / child components.
			if (TryGetField(L, ActorIdx, "Components"))
			{
				const int CompsIdx = lua_absindex(L, -1);
				const int NumComps = (int)lua_rawlen(L, CompsIdx);
				for (int c = 1; c <= NumComps; ++c)
				{
					lua_rawgeti(L, CompsIdx, c);
					if (lua_istable(L, -1))
					{
						const int CompIdx = lua_absindex(L, -1);
						if (CComponent* Comp = ParseComponent(L, CompIdx, Actor, InLevelMaterials))
						{
							// UE: USceneComponent::RelativeLocation / RelativeRotation / RelativeScale3D
							CSceneComponent* SceneComp = dynamic_cast<CSceneComponent*>(Comp);
							if (nullptr != SceneComp && TryGetField(L, CompIdx, "RelativeTransform"))
							{
								Vec3F RL;
								CQuaternion RR;
								Vec3F RS;
								ReadTransform(L, -1, RL, RR, RS);
								lua_pop(L, 1);
								SceneComp->SetLocation(RL);
								SceneComp->SetRotation(RR);
								SceneComp->SetScale(RS);
							}
						}
						else
						{
							lua_pop(L, 1);
						}
					}
					lua_pop(L, 1);
				}
				lua_pop(L, 1);
			}

			return Actor;
		}
	}

	CLevel::CLevel()
	{

	}

	CLevel::~CLevel()
	{

	}

	bool CLevel::Load(const std::string& FilePath, std::vector<FStreamingLevelDesc>* OutStreamingLevels)
	{
		// UE counterpart: ULevel::Serialize deserializes this level's actors off
		// the map package's archive. Here the level is a Lua table literal, so the
		// archive is a throwaway lua_State.
		//
		// This runs on a worker thread (see CWorld::LoadAsync), so the state is
		// local: no shared state, no locking, closed before we return.
		// A reload drops whatever this level currently owns first.
		Unload();

		lua_State* L = luaL_newstate();
		if (nullptr == L)
		{
			OutputDebugStringA("CLevel::Load: luaL_newstate failed\n");
			return false;
		}

		OpenSandboxLibs(L);

		// dofile leaves either an error message or the level table on the stack.
		if (LUA_OK != luaL_dofile(L, FilePath.c_str()))
		{
			OutputDebugStringA((std::string("CLevel::Load: failed to load ") +
				FilePath + " : " + lua_tostring(L, -1) + "\n").c_str());
			lua_close(L);
			return false;
		}

		if (!lua_istable(L, -1))
		{
			OutputDebugStringA("CLevel::Load: level file must return a table\n");
			lua_close(L);
			return false;
		}

		const int LevelIdx = lua_absindex(L, -1);

		// UE: ULevel name (stored on CObject, which owns the name for every object)
		if (TryGetField(L, LevelIdx, "Name"))
		{
			SetName(lua_tostring(L, -1));
			lua_pop(L, 1);
		}

		// Materials declared by this level. Parsed before the actors because
		// OverrideMaterials entries refer to them by name.
		if (TryGetField(L, LevelIdx, "Materials"))
		{
			ParseMaterials(L, lua_absindex(L, -1), LevelMaterials);
			lua_pop(L, 1);
		}

		// UE: ULevel::Actors
		if (TryGetField(L, LevelIdx, "Actors"))
		{
			const int ActorsIdx = lua_absindex(L, -1);
			const int NumActors = (int)lua_rawlen(L, ActorsIdx);
			for (int i = 1; i <= NumActors; ++i)
			{
				lua_rawgeti(L, ActorsIdx, i);
				if (lua_istable(L, -1))
				{
					if (CActor* Actor = ParseActor(L, lua_absindex(L, -1), LevelMaterials))
					{
						AddActor(Actor);
					}
				}
				lua_pop(L, 1);
			}
			lua_pop(L, 1);
		}

		// UE: UWorld::StreamingLevels (ULevelStreaming). Declared in the same map
		// package but owned by the world, so they are handed back rather than stored.
		if (nullptr != OutStreamingLevels && TryGetField(L, LevelIdx, "StreamingLevels"))
		{
			const int StreamIdx = lua_absindex(L, -1);
			const int NumStreams = (int)lua_rawlen(L, StreamIdx);
			for (int i = 1; i <= NumStreams; ++i)
			{
				lua_rawgeti(L, StreamIdx, i);
				const int EntryIdx = lua_absindex(L, -1);

				FStreamingLevelDesc Desc;
				if (TryGetField(L, EntryIdx, "Name"))
				{
					Desc.Name = lua_tostring(L, -1);
					lua_pop(L, 1);
				}
				if (TryGetField(L, EntryIdx, "Path"))
				{
					Desc.Path = lua_tostring(L, -1);
					lua_pop(L, 1);
				}
				if (!Desc.Name.empty() && !Desc.Path.empty())
				{
					OutStreamingLevels->push_back(Desc);
				}
				lua_pop(L, 1);
			}
			lua_pop(L, 1);
		}

		// The level is data only - once it has been turned into C++ actors the VM
		// has done its job. UE keeps no archive alive after ULevel::Serialize either.
		lua_close(L);

		bLoaded = true;
		return true;
	}

	void CLevel::Unload()
	{
		for (CActor* Actor : Actors)
		{
			SafeDelete(Actor);
		}
		Actors.clear();

		// Inline materials are owned by this level (no material assets yet), so
		// they die with it - after the components that referenced them.
		for (auto& Pair : LevelMaterials)
		{
			SafeDelete(Pair.second);
		}
		LevelMaterials.clear();

		bLoaded = false;
	}

	void CLevel::AddActor(CActor* Actor)
	{
		if (Actor)
		{
			Actors.push_back(Actor);
		}
	}

	void CLevel::RemoveActor(CActor* Actor)
	{
		auto item = std::find(Actors.begin(), Actors.end(), Actor);
		if (item != Actors.end())
		{
			Actors.erase(item);
		}
	}

}
