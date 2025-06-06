#pragma once

#include "Core_GraphicsEnums.hpp"

#include <d3d12.h>
#include <dxgi1_6.h>

namespace Atrium::DirectX12
{
	DXGI_FORMAT ToDXGIFormat(const GraphicsFormat& aFormat);
	GraphicsFormat ToGraphicsFormat(const RenderTextureFormat& aRTFormat);
	GraphicsFormat ToGraphicsFormat(const TextureFormat& aFormat);
	D3D12_RESOURCE_DIMENSION ToD3DTextureDimension(const TextureDimension& aDimension);
	D3D12_RTV_DIMENSION ToRTVTextureDimension(const TextureDimension& aDimension);
	D3D12_DSV_DIMENSION ToDSVTextureDimension(const TextureDimension& aDimension);

	TextureFormat ToTextureFormat(const DXGI_FORMAT& aFormat);
}