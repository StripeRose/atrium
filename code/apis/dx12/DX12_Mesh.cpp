// Filter "Resources"

#include "DX12_Mesh.hpp"

#include <DirectXMath.h>

#include <filesystem>
#include <fstream>

// https://alain.xyz/blog/raw-directx12
// https://alextardif.com/Bindless.html

namespace RoseGold::DirectX12
{
	Mesh::Mesh(Manager& aManager)
		: Core::Graphics::Mesh(aManager)
	{ }

	std::shared_ptr<VertexBuffer> Mesh::GetVertexBuffer()
	{
		return std::static_pointer_cast<VertexBuffer>(myVertexBuffer);
	}

	void Mesh::SetFromList(const std::span<Core::Graphics::Vertex> someVertices)
	{
		struct DXVertex
		{
			DirectX::XMFLOAT3 Position;
			DirectX::XMFLOAT3 Normal;
			DirectX::XMFLOAT2 UV;
			DirectX::XMFLOAT3 Color;

			DirectX::XMFLOAT3 Binormal;
			DirectX::XMFLOAT3 Tangent;
		};

		std::vector<DXVertex> dxVertices;
		dxVertices.reserve(someVertices.size());

		for (const Core::Graphics::Vertex& vertex : someVertices)
		{
			DXVertex& dxVertex = dxVertices.emplace_back();
			dxVertex.Position = DirectX::XMFLOAT3(
				vertex.Position.X,
				vertex.Position.Y,
				vertex.Position.Z);
			dxVertex.Normal = DirectX::XMFLOAT3(
				vertex.Normal.X,
				vertex.Normal.Y,
				vertex.Normal.Z);
			dxVertex.UV = DirectX::XMFLOAT2(
				vertex.UV.X,
				vertex.UV.Y);
			dxVertex.Color = DirectX::XMFLOAT3(
				vertex.Color.R,
				vertex.Color.G,
				vertex.Color.B);
			dxVertex.Binormal = DirectX::XMFLOAT3(
				vertex.Binormal.X,
				vertex.Binormal.Y,
				vertex.Binormal.Z);
			dxVertex.Tangent = DirectX::XMFLOAT3(
				vertex.Tangent.X,
				vertex.Tangent.Y,
				vertex.Tangent.Z);
		}

		myVertexBuffer = myGraphicsManager.CreateGraphicsBuffer(
			GraphicsBuffer::Target::Vertex,
			static_cast<std::uint32_t>(dxVertices.size()), sizeof(DXVertex)
		);
		myVertexBuffer->SetData<DXVertex>(dxVertices);
	}
}