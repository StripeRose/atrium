#pragma once

#include "Graphics_Enums.hpp"

#include <d3d12.h>
#include <dxgi1_6.h>

namespace RoseGold::DirectX12
{
	DXGI_FORMAT ToDXGIFormat(const Core::Graphics::GraphicsFormat& aFormat);
	Core::Graphics::GraphicsFormat ToGraphicsFormat(const Core::Graphics::RenderTextureFormat& aRTFormat);
	Core::Graphics::GraphicsFormat ToGraphicsFormat(const Core::Graphics::TextureFormat& aFormat);
	D3D12_RESOURCE_DIMENSION ToD3DTextureDimension(const Core::Graphics::TextureDimension& aDimension);
	D3D12_RTV_DIMENSION ToRTVTextureDimension(const Core::Graphics::TextureDimension& aDimension);
	D3D12_DSV_DIMENSION ToDSVTextureDimension(const Core::Graphics::TextureDimension& aDimension);

	Core::Graphics::TextureFormat ToTextureFormat(const DXGI_FORMAT& aFormat);
}