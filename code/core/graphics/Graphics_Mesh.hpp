#pragma once

#include "Graphics_Buffer.hpp"
#include "Graphics_MeshPrimitives.hpp"
#include "Graphics_Pipeline.hpp"

#include "Common_Color.hpp"
#include "Common_Math.hpp"

#include <cstdint>
#include <span>

namespace RoseGold::Core::Graphics
{
	struct Triangle
	{
		std::uint32_t V1;
		std::uint32_t V2;
		std::uint32_t V3;
	};

	class Manager;
	class Mesh
	{
	public:
		Mesh(Manager& aGraphicsManager)
			: myGraphicsManager(aGraphicsManager)
		{ }
		virtual ~Mesh() = default;

		virtual std::vector<PipelineState::InputLayoutEntry> GetInputLayout() const = 0;

		std::shared_ptr<const GraphicsBuffer> GetVertexBuffer() const { return myVertexBuffer; }

		virtual void SetFromPrimitive(MeshPrimitiveType aPrimitive) = 0;

	protected:
		std::shared_ptr<GraphicsBuffer> myVertexBuffer;
		Manager& myGraphicsManager;
	};

	class GenericMesh : public Mesh
	{
	public:
		struct Vertex
		{
			Math::Vector3 Position;
			Math::Vector3 Normal;
			Math::Vector3 Binormal;
			Math::Vector3 Tangent;
			Math::Vector2 UV;
		};

	public:
		GenericMesh(Manager& aGraphicsManager);

		std::vector<PipelineState::InputLayoutEntry> GetInputLayout() const override;

		void SetFromList(const std::span<Vertex> someVertices);
		void SetFromList(const std::span<Vertex> someVertices, const std::span<Triangle> someTriangles);

		void SetFromPrimitive(MeshPrimitiveType aType) override;
	};

	class ColoredMesh : public Mesh
	{
	public:
		struct Vertex
		{
			Math::Vector3 Position;
			Math::Vector3 Normal;
			Math::Vector3 Binormal;
			Math::Vector3 Tangent;
			Math::Vector2 UV;
			Color Color;
		};

	public:
		ColoredMesh(Manager& aGraphicsManager);

		std::vector<PipelineState::InputLayoutEntry> GetInputLayout() const override;

		void SetFromList(const std::span<Vertex> someVertices);
		void SetFromList(const std::span<Vertex> someVertices, const std::span<Triangle> someTriangles);

		void SetFromPrimitive(MeshPrimitiveType aType) override;
	};

	class SimpleMesh : public Mesh
	{
	public:
		struct Vertex
		{
			Math::Vector3 Position;
		};

	public:
		SimpleMesh(Manager& aGraphicsManager);

		std::vector<PipelineState::InputLayoutEntry> GetInputLayout() const override;

		void SetFromList(const std::span<Vertex> someVertices);
		void SetFromList(const std::span<Vertex> someVertices, const std::span<Triangle> someTriangles);

		void SetFromPrimitive(MeshPrimitiveType aType) override;
	};

	/*class FullscreenMesh : public SimpleMesh
	{
	public:
		FullscreenMesh(Manager& aGraphicsManager);
	};*/
}