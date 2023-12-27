#include "Graphics_Mesh.hpp"

#include "Graphics_Manager.hpp"

#include <rose-common/math/Common.hpp>

#include <vector>

namespace RoseGold::Core::Graphics
{
	Vertex Vertex::Lerp(const Vertex& aVertex, const Vertex& anOtherVertex, const float anAmount)
	{
		Vertex vertex;
		vertex.Position = Math::Vector3::Lerp(aVertex.Position, anOtherVertex.Position, anAmount);
		vertex.Normal = Math::Vector3::Slerp(aVertex.Normal, anOtherVertex.Normal, anAmount);
		vertex.UV = Math::Vector2::Lerp(aVertex.UV, anOtherVertex.UV, anAmount);

		vertex.Binormal = Math::Vector3::Slerp(aVertex.Binormal, anOtherVertex.Binormal, anAmount);
		vertex.Tangent = Math::Vector3::Slerp(aVertex.Tangent, anOtherVertex.Tangent, anAmount);
		vertex.Color = RoseCommon::Math::Lerp(aVertex.Color, anOtherVertex.Color, anAmount);

		return vertex;
	}

	std::vector<PipelineState::InputLayoutEntry> Mesh::GetInputLayout() const
	{
		std::vector<PipelineState::InputLayoutEntry> inputLayout;

		inputLayout.emplace_back("POSITION", GraphicsFormat::R32G32B32_SFloat);
		inputLayout.emplace_back("NORMAL", GraphicsFormat::R32G32B32_SFloat);
		inputLayout.emplace_back("TEXCOORD", GraphicsFormat::R32G32_SFloat);
		inputLayout.emplace_back("COLOR", GraphicsFormat::R32G32B32_SFloat);
		inputLayout.emplace_back("BINORMAL", GraphicsFormat::R32G32B32_SFloat);
		inputLayout.emplace_back("TANGENT", GraphicsFormat::R32G32B32_SFloat);

		return inputLayout;
	}

	void Mesh::SetFromList(const std::span<Vertex> someVertices)
	{
		if (someVertices.empty())
			return;

		struct ConvertedVertex
		{
			Math::Vector3 Position;
			Math::Vector3 Normal;
			Math::Vector2 UV;
			Math::Vector3 Color;
			Math::Vector3 Binormal;
			Math::Vector3 Tangent;
		};

		std::vector<ConvertedVertex> convertedVertices;
		convertedVertices.reserve(someVertices.size());

		for (const Core::Graphics::Vertex& vertex : someVertices)
		{
			ConvertedVertex& convertedVertex = convertedVertices.emplace_back();
			convertedVertex.Position = vertex.Position;
			convertedVertex.Normal = vertex.Normal;
			convertedVertex.UV = vertex.UV;
			convertedVertex.Color = Math::Vector3(vertex.Color.R, vertex.Color.G, vertex.Color.B);
			convertedVertex.Binormal = vertex.Binormal;
			convertedVertex.Tangent = vertex.Tangent;
		}

		myVertexBuffer = myGraphicsManager.CreateGraphicsBuffer(
			GraphicsBuffer::Target::Vertex,
			static_cast<std::uint32_t>(convertedVertices.size()), sizeof(ConvertedVertex)
		);
		myVertexBuffer->SetData<ConvertedVertex>(convertedVertices);
	}

	void Mesh::SetFromList(const std::span<Vertex> someVertices, const std::span<Triangle> someTriangles)
	{
		if (someTriangles.empty() && !someVertices.empty())
		{
			SetFromList(someVertices);
			return;
		}

		std::vector<Vertex> vertexList;
		for (const Triangle& tri : someTriangles)
		{
			vertexList.push_back(someVertices[tri.V1]);
			vertexList.push_back(someVertices[tri.V2]);
			vertexList.push_back(someVertices[tri.V3]);
		}

		SetFromList(vertexList);
	}
}
