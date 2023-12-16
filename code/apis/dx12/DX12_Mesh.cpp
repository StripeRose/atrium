#include "DX12_Mesh.hpp"

#include "DX12_Manager.hpp"

namespace RoseGold::DirectX12
{
	Mesh::Mesh(Manager& aManager)
		: Core::Graphics::Mesh(aManager)
	{ }
}