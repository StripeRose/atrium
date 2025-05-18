// Filter "Graphics"

#pragma once

#include "Atrium_Math.hpp"

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
			Vector3 Position;
			Vector3 Normal;
			Vector3 Binormal;
			Vector3 Tangent;
			Vector2 UV;
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