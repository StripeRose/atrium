// Filter "Chart/Rendering"
#include "stdafx.hpp"
#include "ChartMeshes.hpp"

std::unique_ptr<Mesh> CreateFretboardMesh(RoseGold::Core::GraphicsAPI& aGraphicsAPI)
{
	std::unique_ptr<ChartFretboardMesh> mesh(new ChartFretboardMesh(aGraphicsAPI));
	std::vector<ChartFretboardVertex> vertices;

	vertices.emplace_back(ChartFretboardVertex{ -1, 0, 0 });
	vertices.emplace_back(ChartFretboardVertex{ -1, 0, 5 });
	vertices.emplace_back(ChartFretboardVertex{ 1, 0, 0 });

	vertices.emplace_back(ChartFretboardVertex{ 1, 0, 0 });
	vertices.emplace_back(ChartFretboardVertex{ -1, 0, 5 });
	vertices.emplace_back(ChartFretboardVertex{ 1, 0, 5 });

	mesh->SetVertices(vertices);
	return mesh;
}
