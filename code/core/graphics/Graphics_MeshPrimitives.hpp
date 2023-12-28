#pragma once

#include "Common_Math.hpp"

#include <functional>
#include <vector>

namespace RoseGold::Core::Graphics
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

	struct MeshPrimitive
	{
		struct Vertex
		{
			Math::Vector3 Position;
			Math::Vector3 Normal;
			Math::Vector3 Binormal;
			Math::Vector3 Tangent;
			Math::Vector2 UV;
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