// Filter "Graphics"

#pragma once

#include <rose-common/math/Vector.hpp>

#include <functional>
#include <vector>

namespace Atrium::Core
{
	enum class MeshPrimitiveType
	{
		Capsule,
		Cube,
		Cylinder,
		Disc,
		Plane,
		Sphere,
		Icosphere,
		Quad
	};

	/**
	 * @brief Generates and describes a mesh primitive.
	 */
	struct MeshPrimitive
	{
		struct Vertex
		{
			Vector3T<float> Position;
			Vector3T<float> Normal;
			Vector3T<float> Binormal;
			Vector3T<float> Tangent;
			Vector2T<float> UV;
		};

		struct Triangle
		{
			std::uint32_t V1;
			std::uint32_t V2;
			std::uint32_t V3;
		};

		std::vector<Vertex> Vertices;
		std::vector<Triangle> Triangles;

		static MeshPrimitive Generate(MeshPrimitiveType aType);
	};
}