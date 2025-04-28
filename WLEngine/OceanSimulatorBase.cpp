#include "OceanSimulatorBase.h"
#include "MeshInstance.h"

#define HALF_SQRT_2	0.7071068f
#define GRAV_ACCEL	981.0f	// The acceleration of gravity, cm/s^2

namespace WL
{
	bool isLeaf(const QuadNode& quad_node)
	{
		return (quad_node.sub_node[0] == -1 && quad_node.sub_node[1] == -1 && quad_node.sub_node[2] == -1 && quad_node.sub_node[3] == -1);
	}
	
	int searchLeaf(const std::vector<QuadNode>& node_list, const Vec2F& point)
	{
		int index = -1;

		int size = (int)node_list.size();
		QuadNode node = node_list[size - 1];

		while (!isLeaf(node))
		{
			bool found = false;

			for (int i = 0; i < 4; i++)
			{
				index = node.sub_node[i];
				if (index == -1)
				{
					continue;
				}
				QuadNode sub_node = node_list[index];
				if (point.x >= sub_node.bottom_left.x && point.x <= sub_node.bottom_left.x + sub_node.length &&
					point.y >= sub_node.bottom_left.y && point.y <= sub_node.bottom_left.y + sub_node.length)
				{
					node = sub_node;
					found = true;
					break;
				}
			}

			if (!found)
			{
				return -1;
			}
		}

		return index;
	}

	// Generating gaussian random number with mean 0 and standard deviation 1.
	float Gauss()
	{
		float u1 = rand() / (float)RAND_MAX;
		float u2 = rand() / (float)RAND_MAX;
		if (u1 < 1e-6f)
		{
			u1 = 1e-6f;
		}
		return sqrtf(-2 * logf(u1)) * cosf(2 * PI_F * u2);
	}

	// Phillips Spectrum
	// K: normalized wave vector, W: wind direction, v: wind velocity, a: amplitude constant
	float Phillips(Vec2F K, Vec2F W, float v, float a, float dir_depend)
	{
		// largest possible wave from constant wind of velocity v
		float l = v * v / GRAV_ACCEL;
		// damp out waves with very small length w << l
		float w = l / 1000;

		float Ksqr = K.x * K.x + K.y * K.y;
		float Kcos = K.x * W.x + K.y * W.y;
		float phillips = a * expf(-1 / (l * l * Ksqr)) / (Ksqr * Ksqr * Ksqr) * (Kcos * Kcos);

		// filter out waves moving opposite to wind
		if (Kcos < 0)
		{
			phillips *= dir_depend;
		}
		// damp out waves with very small length w << l
		return phillips * expf(-Ksqr * w * w);
	}

	void OceanSimulatorBase::initParam()
	{
		// The size of displacement map. In this sample, it's fixed to 512.
		mParam.dmap_dim = 512;
		// The side length (world space) of square patch
		mParam.patch_length = 2000.0f;
		// Adjust this parameter to control the simulation speed
		mParam.time_scale = 0.8f;
		// A scale to control the amplitude. Not the world space height
		mParam.wave_amplitude = 0.35f;
		// 2D wind direction. No need to be normalized
		mParam.wind_dir = Vec2F(0.8f, 0.6f);
		// The bigger the wind speed, the larger scale of wave crest.
		// But the wave scale can be no larger than patch_length
		mParam.wind_speed = 600.0f;
		// Damp out the components opposite to wind direction.
		// The smaller the value, the higher wind dependency
		mParam.wind_dependency = 0.07f;
		// Control the scale of horizontal movement. Higher value creates
		// pointy crests.
		mParam.choppy_scale = 1.3f;
	}

	void OceanSimulatorBase::initHeightMap(Vec2F* pOutH0, float* pOutOmega)
	{
		Vec2F K;

		Vec2F wind_dir = mParam.wind_dir;
		wind_dir.normalize();
		float a = mParam.wave_amplitude * 1e-7f;	// It is too small. We must scale it for editing.
		float v = mParam.wind_speed;
		float dir_depend = mParam.wind_dependency;

		int height_map_dim = mParam.dmap_dim;
		float patch_length = mParam.patch_length;

		for (int i = 0; i <= height_map_dim; i++)
		{
			// K is wave-vector, range [-|DX/W, |DX/W], [-|DY/H, |DY/H]
			K.y = (-height_map_dim / 2.0f + i) * (2 * PI_F / patch_length);

			for (int j = 0; j <= height_map_dim; j++)
			{
				K.x = (-height_map_dim / 2.0f + j) * (2 * PI_F / patch_length);

				float phil = (K.x == 0 && K.y == 0) ? 0 : sqrtf(Phillips(K, wind_dir, v, a, dir_depend));

				pOutH0[i * (height_map_dim + 4) + j].x = float(phil * Gauss() * HALF_SQRT_2);
				pOutH0[i * (height_map_dim + 4) + j].y = float(phil * Gauss() * HALF_SQRT_2);

				// The angular frequency is following the dispersion relation:
				//            out_omega^2 = g*k
				// The equation of Gerstner wave:
				//            x = x0 - K/k * A * sin(dot(K, x0) - sqrt(g * k) * t), x is a 2D vector.
				//            z = A * cos(dot(K, x0) - sqrt(g * k) * t)
				// Gerstner wave shows that a point on a simple sinusoid wave is doing a uniform circular
				// motion with the center (x0, y0, z0), radius A, and the circular plane is parallel to
				// vector K.
				pOutOmega[i * (height_map_dim + 4) + j] = sqrtf(GRAV_ACCEL * sqrtf(K.x * K.x + K.y * K.y));
			}
		}
	}

	//////////////////////////////////////////////////////////////////////////
	QuadNode::~QuadNode()
	{
		for (auto item : pMeshInstanceInfo)
		{
			WL_DELETE(item, Default);
		}
	}
}
