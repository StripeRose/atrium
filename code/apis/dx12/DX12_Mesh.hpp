#pragma once

#include <Graphics_Mesh.hpp>

namespace RoseGold::DirectX12
{
	class Manager;
	class Mesh : public Core::Graphics::Mesh
	{
	public:
		Mesh(Manager& aManager);
		~Mesh() = default;
	};
}
