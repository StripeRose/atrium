// Filter "Chart/Rendering"
#pragma once

#include "Mesh.hpp"

struct ChartFretboardVertex
{
	static inline std::vector<RoseGold::Core::PipelineStateDescription::InputLayoutEntry> GetInputLayout()
	{
		std::vector<RoseGold::Core::PipelineStateDescription::InputLayoutEntry> layout;
		layout.emplace_back("POSITION", RoseGold::Core::GraphicsFormat::R32G32B32_SFloat);
		//layout.emplace_back("TEXCOORD", RoseGold::Core::GraphicsFormat::R32G32_SFloat);
		return layout;
	}

	float Position[3];
	//float UV[2];
};
using ChartFretboardMesh = MeshT<ChartFretboardVertex>;

std::unique_ptr<Mesh> CreateFretboardMesh(RoseGold::Core::GraphicsAPI& aGraphicsAPI);