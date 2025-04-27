#pragma once

#include "Atrium_GraphicsEnums.hpp"

#include <d3d12.h>
#include <dxgi1_6.h>

namespace Atrium::DirectX12
{
	DXGI_FORMAT ToDXGIFormat(const Core::GraphicsFormat& aFormat);
	Core::GraphicsFormat ToGraphicsFormat(const Core::RenderTextureFormat& aRTFormat);
	Core::GraphicsFormat ToGraphicsFormat(const Core::TextureFormat& aFormat);
	D3D12_RESOURCE_DIMENSION ToD3DTextureDimension(const Core::TextureDimension& aDimension);
	D3D12_RTV_DIMENSION ToRTVTextureDimension(const Core::TextureDimension& aDimension);
	D3D12_DSV_DIMENSION ToDSVTextureDimension(const Core::TextureDimension& aDimension);

	Core::TextureFormat ToTextureFormat(const DXGI_FORMAT& aFormat);
}