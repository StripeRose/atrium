// Filter "Graphics"

#pragma once

#include <rose-common/math/Vector.hpp>

#include <functional>
#include <vector>

namespace Atrium
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
			Vector3<float> Position;
			Vector3<float> Normal;
			Vector3<float> Binormal;
			Vector3<float> Tangent;
			Vector2<float> UV;
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