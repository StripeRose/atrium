#pragma once

#include <rose-common/SemanticVersion.hpp>

#include <string>

namespace Atrium
{
	struct ApplicationParameters
	{
		std::string Name;
		Atrium::SemanticVersion Version;

		enum GraphicsHandler
		{
			None,

		#ifdef ATRIUM_DX12
			DirectX12,
		#endif

		#ifdef ATRIUM_VULKAN
			Vulkan,
		#endif
		};

		GraphicsHandler Graphics = GraphicsHandler::None;
	};
}
