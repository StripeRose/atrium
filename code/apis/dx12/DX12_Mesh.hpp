// Filter "Resources"

#pragma once

#include "DX12_ComPtr.hpp"
#include "DX12_GraphicsBuffer.hpp"
#include "DX12_Manager.hpp"
#include "DX12_Shader.hpp"

#include <Graphics_Mesh.hpp>

#include <d3d12.h>

namespace RoseGold::DirectX12
{
	class Manager;
	class Mesh : public Core::Graphics::Mesh
	{
	public:
		Mesh(Manager& aManager);
		~Mesh() = default;

		std::shared_ptr<VertexBuffer> GetVertexBuffer();

		void SetFromList(const std::span<Core::Graphics::Vertex> someVertices) override;
	};
}
