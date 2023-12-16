#include "Graphics_Mesh.hpp"

#include "Graphics_Manager.hpp"

#include <rose-common/math/Common.hpp>

#include <vector>

namespace RoseGold::Core::Graphics
{
	/*Vertex Vertex::Lerp(const Vertex& aVertex, const Vertex& anOtherVertex, const float anAmount)
	{
		Vertex vertex;
		vertex.Position = RoseCommon::Math::Lerp(aVertex.Position, anOtherVertex.Position, anAmount);
		vertex.Normal = System::Math::Vector3::Slerp(aVertex.Normal, anOtherVertex.Normal, anAmount);
		vertex.UV = RoseCommon::Math::Lerp(aVertex.UV, anOtherVertex.UV, anAmount);

		vertex.Binormal = System::Math::Vector3::Slerp(aVertex.Binormal, anOtherVertex.Binormal, anAmount);
		vertex.Tangent = System::Math::Vector3::Slerp(aVertex.Tangent, anOtherVertex.Tangent, anAmount);

		return vertex;
	}*/

	void Mesh::SetFromList(const std::span<Vertex> someVertices)
	{
		myVertexBuffer = myGraphicsManager.CreateGraphicsBuffer(
			GraphicsBuffer::Target::Vertex,
			static_cast<std::uint32_t>(someVertices.size()), sizeof(Vertex)
		);
		myVertexBuffer->SetData<Vertex>(someVertices);
	}

	void Mesh::SetFromList(const std::span<Vertex> someVertices, const std::span<Triangle> someTriangles)
	{
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
