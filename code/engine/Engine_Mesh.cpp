// Filter "Graphics"
#include "stdafx.hpp"

#include "Engine_Mesh.hpp"

#include "Graphics_Manager.hpp"

#include <rose-common/math/Common.hpp>

#include <vector>

//namespace RoseGold::Core::Graphics
//{
//	GenericMesh::GenericMesh(Manager& aGraphicsManager)
//		: Mesh(aGraphicsManager)
//	{ }
//
//	std::vector<PipelineStateDescription::InputLayoutEntry> GenericMesh::GetInputLayout() const
//	{
//		std::vector<PipelineStateDescription::InputLayoutEntry> inputLayout;
//
//		inputLayout.emplace_back("POSITION", GraphicsFormat::R32G32B32_SFloat);
//		inputLayout.emplace_back("NORMAL", GraphicsFormat::R32G32B32_SFloat);
//		inputLayout.emplace_back("BINORMAL", GraphicsFormat::R32G32B32_SFloat);
//		inputLayout.emplace_back("TANGENT", GraphicsFormat::R32G32B32_SFloat);
//		inputLayout.emplace_back("TEXCOORD", GraphicsFormat::R32G32_SFloat);
//
//		return inputLayout;
//	}
//
//	void GenericMesh::SetFromList(const std::span<Vertex> someVertices)
//	{
//		if (someVertices.empty())
//			return;
//
//		myVertexCount = static_cast<std::uint32_t>(someVertices.size());
//		myVertexBuffer = myGraphicsManager.CreateGraphicsBuffer(
//			GraphicsBuffer::Target::Vertex,
//			myVertexCount, sizeof(Vertex)
//		);
//		myVertexBuffer->SetData<Vertex>(someVertices);
//	}
//
//	void GenericMesh::SetFromList(const std::span<Vertex> someVertices, const std::span<Triangle> someTriangles)
//	{
//		if (someTriangles.empty() && !someVertices.empty())
//		{
//			SetFromList(someVertices);
//			return;
//		}
//
//		std::vector<Vertex> vertexList;
//		for (const Triangle& tri : someTriangles)
//		{
//			vertexList.push_back(someVertices[tri.V1]);
//			vertexList.push_back(someVertices[tri.V2]);
//			vertexList.push_back(someVertices[tri.V3]);
//		}
//
//		SetFromList(vertexList);
//	}
//
//	void GenericMesh::SetFromPrimitive(MeshPrimitiveType aType)
//	{
//		MeshPrimitive primitiveData = MeshPrimitive::Generate(aType);
//
//		std::vector<Vertex> vertices;
//		vertices.reserve(primitiveData.Vertices.size());
//		for (const MeshPrimitive::Vertex& primitiveVertex : primitiveData.Vertices)
//		{
//			Vertex& vertex = vertices.emplace_back();
//			vertex.Position = primitiveVertex.Position;
//			vertex.Normal = primitiveVertex.Normal;
//			vertex.Binormal = primitiveVertex.Binormal;
//			vertex.Tangent = primitiveVertex.Tangent;
//			vertex.UV = primitiveVertex.UV;
//		}
//
//		std::vector<Triangle> triangles;
//		triangles.reserve(primitiveData.Triangles.size());
//		for (const MeshPrimitive::Triangle& primitiveTriangle : primitiveData.Triangles)
//		{
//			triangles.emplace_back(
//				Triangle {
//					primitiveTriangle.V1,
//					primitiveTriangle.V2,
//					primitiveTriangle.V3
//				}
//			);
//		}
//
//		SetFromList(vertices, triangles);
//	}
//
//	ColoredMesh::ColoredMesh(Manager& aGraphicsManager)
//		: Mesh(aGraphicsManager)
//	{ }
//
//	std::vector<PipelineStateDescription::InputLayoutEntry> ColoredMesh::GetInputLayout() const
//	{
//		std::vector<PipelineStateDescription::InputLayoutEntry> inputLayout;
//
//		inputLayout.emplace_back("POSITION", GraphicsFormat::R32G32B32_SFloat);
//		inputLayout.emplace_back("NORMAL", GraphicsFormat::R32G32B32_SFloat);
//		inputLayout.emplace_back("BINORMAL", GraphicsFormat::R32G32B32_SFloat);
//		inputLayout.emplace_back("TANGENT", GraphicsFormat::R32G32B32_SFloat);
//		inputLayout.emplace_back("TEXCOORD", GraphicsFormat::R32G32_SFloat);
//		inputLayout.emplace_back("COLOR", GraphicsFormat::R32G32B32A32_SFloat);
//
//		return inputLayout;
//	}
//
//	void ColoredMesh::SetFromList(const std::span<Vertex> someVertices)
//	{
//		if (someVertices.empty())
//			return;
//
//		// Color is ARGB, shader is RGBA, just reorganize a bit.
//		std::vector<Vertex> adjustedVertices = std::vector<Vertex>(someVertices.begin(), someVertices.end());
//		for (Vertex& v : adjustedVertices)
//			v.Color = Color(v.Color.R, v.Color.G, v.Color.B, v.Color.A);
//
//		myVertexCount = static_cast<std::uint32_t>(adjustedVertices.size());
//		myVertexBuffer = myGraphicsManager.CreateGraphicsBuffer(
//			GraphicsBuffer::Target::Vertex,
//			myVertexCount, sizeof(Vertex)
//		);
//		myVertexBuffer->SetData<Vertex>(adjustedVertices);
//	}
//
//	void ColoredMesh::SetFromList(const std::span<Vertex> someVertices, const std::span<Triangle> someTriangles)
//	{
//		if (someTriangles.empty() && !someVertices.empty())
//		{
//			SetFromList(someVertices);
//			return;
//		}
//
//		std::vector<Vertex> vertexList;
//		for (const Triangle& tri : someTriangles)
//		{
//			vertexList.push_back(someVertices[tri.V1]);
//			vertexList.push_back(someVertices[tri.V2]);
//			vertexList.push_back(someVertices[tri.V3]);
//		}
//
//		SetFromList(vertexList);
//	}
//
//	void ColoredMesh::SetFromPrimitive(MeshPrimitiveType aType)
//	{
//		MeshPrimitive primitiveData = MeshPrimitive::Generate(aType);
//
//		std::vector<Vertex> vertices;
//		vertices.reserve(primitiveData.Vertices.size());
//		for (const MeshPrimitive::Vertex& primitiveVertex : primitiveData.Vertices)
//		{
//			Vertex& vertex = vertices.emplace_back();
//			vertex.Position = primitiveVertex.Position;
//			vertex.Normal = primitiveVertex.Normal;
//			vertex.Binormal = primitiveVertex.Binormal;
//			vertex.Tangent = primitiveVertex.Tangent;
//			vertex.UV = primitiveVertex.UV;
//			vertex.Color = Color(1.f, vertex.UV.X, vertex.UV.Y, 1.f);
//		}
//
//		std::vector<Triangle> triangles;
//		triangles.reserve(primitiveData.Triangles.size());
//		for (const MeshPrimitive::Triangle& primitiveTriangle : primitiveData.Triangles)
//		{
//			triangles.emplace_back(
//				Triangle{
//					primitiveTriangle.V1,
//					primitiveTriangle.V2,
//					primitiveTriangle.V3
//				}
//			);
//		}
//
//		SetFromList(vertices, triangles);
//	}
//
//	SimpleMesh::SimpleMesh(Manager& aGraphicsManager)
//		: Mesh(aGraphicsManager)
//	{ }
//
//	std::vector<PipelineStateDescription::InputLayoutEntry> SimpleMesh::GetInputLayout() const
//	{
//		std::vector<PipelineStateDescription::InputLayoutEntry> inputLayout;
//
//		inputLayout.emplace_back("POSITION", GraphicsFormat::R32G32B32_SFloat);
//
//		return inputLayout;
//	}
//
//	void SimpleMesh::SetFromList(const std::span<Vertex> someVertices)
//	{
//		if (someVertices.empty())
//			return;
//
//		myVertexCount = static_cast<std::uint32_t>(someVertices.size());
//		myVertexBuffer = myGraphicsManager.CreateGraphicsBuffer(
//			GraphicsBuffer::Target::Vertex,
//			myVertexCount, sizeof(Vertex)
//		);
//		myVertexBuffer->SetData<Vertex>(someVertices);
//	}
//
//	void SimpleMesh::SetFromList(const std::span<Vertex> someVertices, const std::span<Triangle> someTriangles)
//	{
//		if (someTriangles.empty() && !someVertices.empty())
//		{
//			SetFromList(someVertices);
//			return;
//		}
//
//		std::vector<Vertex> vertexList;
//		for (const Triangle& tri : someTriangles)
//		{
//			vertexList.push_back(someVertices[tri.V1]);
//			vertexList.push_back(someVertices[tri.V2]);
//			vertexList.push_back(someVertices[tri.V3]);
//		}
//
//		SetFromList(vertexList);
//	}
//
//	void SimpleMesh::SetFromPrimitive(MeshPrimitiveType aType)
//	{
//		MeshPrimitive primitiveData = MeshPrimitive::Generate(aType);
//
//		std::vector<Vertex> vertices;
//		vertices.reserve(primitiveData.Vertices.size());
//		for (const MeshPrimitive::Vertex& primitiveVertex : primitiveData.Vertices)
//		{
//			Vertex& vertex = vertices.emplace_back();
//			vertex.Position = primitiveVertex.Position;
//		}
//
//		std::vector<Triangle> triangles;
//		triangles.reserve(primitiveData.Triangles.size());
//		for (const MeshPrimitive::Triangle& primitiveTriangle : primitiveData.Triangles)
//		{
//			triangles.emplace_back(
//				Triangle{
//					primitiveTriangle.V1,
//					primitiveTriangle.V2,
//					primitiveTriangle.V3
//				}
//			);
//		}
//
//		SetFromList(vertices, triangles);
//	}
//}
