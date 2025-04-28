#pragma once
#include "Device.h"
#define CS_DEBUG_BUFFER
#define BLOCK_SIZE_X 16
#define BLOCK_SIZE_Y 16
#define COHERENCY_GRANULARITY 128

namespace WL
{
	// Constant buffer
	struct Const_Per_Call
	{
		Matrix44	g_matLocal;
		Matrix44	g_matWorldViewProj;
		Vec2F		g_UVBase;
		Vec2F		g_PerlinMovement;
		Vec3F		g_LocalEye;
	};

	struct Const_Shading
	{
		// Water-reflected sky color
		Vec3F		g_SkyColor;
		float		unused0;
		// The color of bottomless water body
		Vec3F		g_WaterbodyColor;

		// The strength, direction and color of sun streak
		float		g_Shineness;
		Vec3F		g_SunDir;
		float		unused1;
		Vec3F		g_SunColor;
		float		unused2;

		// The parameter is used for fixing an artifact
		Vec3F		g_BendParam;

		// Perlin noise for distant wave crest
		float		g_PerlinSize;
		Vec3F		g_PerlinAmplitude;
		float		unused3;
		Vec3F		g_PerlinOctave;
		float		unused4;
		Vec3F		g_PerlinGradient;

		// Constants for calculating texcoord from position
		float		g_TexelLength_x2;
		float		g_UVScale;
		float		g_UVOffset;
	};

	struct OceanParameter
	{
		// Must be power of 2.
		int dmap_dim;
		// Typical value is 1000 ~ 2000
		float patch_length;

		// Adjust the time interval for simulation.
		float time_scale;
		// Amplitude for transverse wave. Around 1.0
		float wave_amplitude;
		// Wind direction. Normalization not required.
		Vec2F wind_dir;
		// Around 100 ~ 1000
		float wind_speed;
		// This value damps out the waves against the wind direction.
		// Smaller value means higher wind dependency.
		float wind_dependency;
		// The amplitude for longitudinal wave. Must be positive.
		float choppy_scale;
	};

	struct OceanSimulatorBase
	{
		OceanParameter	mParam;
		Const_Shading   shadingData;
		Const_Per_Call  callConsts;
		void initParam();
		void initHeightMap(Vec2F* pOutH0, float* pOutOmega);
	};

	struct MeshInstanceInfo;
	// Quadtree structures & routines
	struct QuadNode
	{
		Vec2F bottom_left;
		float length;
		int lod;

		int sub_node[4];

		std::array<MeshInstanceInfo*, 2> pMeshInstanceInfo;
		~QuadNode();

	};

	struct QuadRenderParam
	{
		UINT num_inner_verts;
		UINT num_inner_faces;
		UINT inner_start_index;

		UINT num_boundary_verts;
		UINT num_boundary_faces;
		UINT boundary_start_index;
	};

	extern bool isLeaf(const QuadNode& quad_node);
	extern int searchLeaf(const std::vector<QuadNode>& node_list, const Vec2F& point);
}
