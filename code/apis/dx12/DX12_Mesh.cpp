// Filter "Resources"

#include "DX12_Mesh.hpp"

#include <filesystem>
#include <fstream>

// https://alain.xyz/blog/raw-directx12
// https://alextardif.com/Bindless.html

namespace RoseGold::DirectX12
{
	Mesh::Mesh(Manager& aManager)
		: Core::Graphics::Mesh(aManager)
	{ }
}