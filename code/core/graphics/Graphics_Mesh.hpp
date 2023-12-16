#pragma once

#include "Graphics_Buffer.hpp"

#include "Common_Color.hpp"
#include "Common_Math.hpp"

#include <cstdint>
#include <span>

namespace RoseGold::Core::Graphics
{
	enum class MeshPrimitive
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

	struct Triangle
	{
		std::uint32_t V1;
		std::uint32_t V2;
		std::uint32_t V3;
	};

	struct Vertex
	{
		//static Vertex Lerp(const Vertex& aVertex, const Vertex& anOtherVertex, const float anAmount);

		Math::Vector3 Position;
		Math::Vector3 Normal;
		Math::Vector2 UV;
		Color Color;

		Math::Vector3 Binormal;
		Math::Vector3 Tangent;
	};

	class Manager;
	class Mesh
	{
	public:
		virtual ~Mesh() = default;

		void SetFromList(const std::span<Vertex> someVertices);
		void SetFromList(const std::span<Vertex> someVertices, const std::span<Triangle> someTriangles);

	protected:
		Mesh(Manager& aGraphicsManager)
			: myGraphicsManager(aGraphicsManager)
		{ }

	private:
		std::shared_ptr<GraphicsBuffer> myVertexBuffer;
		Manager& myGraphicsManager;
	};
}