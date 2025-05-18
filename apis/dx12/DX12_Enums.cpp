#include "DX12_Diagnostics.hpp"
#include "DX12_Enums.hpp"

namespace Atrium::DirectX12
{
	DXGI_FORMAT ToDXGIFormat(const Core::GraphicsFormat& aFormat)
	{
		switch (aFormat)
		{
			case Core::GraphicsFormat::None:                     return DXGI_FORMAT_UNKNOWN;

			case Core::GraphicsFormat::R8_UNorm:                 return DXGI_FORMAT_R8_UNORM;
			case Core::GraphicsFormat::R8G8_UNorm:               return DXGI_FORMAT_R8G8_UNORM;
			case Core::GraphicsFormat::R8G8B8_UNorm:             return DXGI_FORMAT_R8G8B8A8_UNORM;
			case Core::GraphicsFormat::R8G8B8A8_UNorm:           return DXGI_FORMAT_R8G8B8A8_UNORM;
			case Core::GraphicsFormat::R8_SNorm:                 return DXGI_FORMAT_R8_SNORM;
			case Core::GraphicsFormat::R8G8_SNorm:               return DXGI_FORMAT_R8G8_SNORM;
			case Core::GraphicsFormat::R8G8B8_SNorm:             return DXGI_FORMAT_UNKNOWN;// Not supported.
			case Core::GraphicsFormat::R8G8B8A8_SNorm:           return DXGI_FORMAT_R8G8B8A8_SNORM;
			case Core::GraphicsFormat::R8_UInt:                  return DXGI_FORMAT_R8_UINT;
			case Core::GraphicsFormat::R8G8_UInt:                return DXGI_FORMAT_R8G8_UINT;
			case Core::GraphicsFormat::R8G8B8_UInt:              return DXGI_FORMAT_UNKNOWN;// Not supported.
			case Core::GraphicsFormat::R8G8B8A8_UInt:            return DXGI_FORMAT_R8G8B8A8_UINT;
			case Core::GraphicsFormat::R8_SInt:                  return DXGI_FORMAT_R8_SINT;
			case Core::GraphicsFormat::R8G8_SInt:                return DXGI_FORMAT_R8G8_SINT;
			case Core::GraphicsFormat::R8G8B8_SInt:              return DXGI_FORMAT_UNKNOWN;// Not supported.
			case Core::GraphicsFormat::R8G8B8A8_SInt:            return DXGI_FORMAT_R8G8B8A8_SINT;

			case Core::GraphicsFormat::R16_UNorm:                return DXGI_FORMAT_R16_UNORM;
			case Core::GraphicsFormat::R16G16_UNorm:             return DXGI_FORMAT_R16G16_UNORM;
			case Core::GraphicsFormat::R16G16B16_UNorm:          return DXGI_FORMAT_UNKNOWN;// Not supported.
			case Core::GraphicsFormat::R16G16B16A16_UNorm:       return DXGI_FORMAT_R16G16B16A16_UNORM;
			case Core::GraphicsFormat::R16_SNorm:                return DXGI_FORMAT_R16_SNORM;
			case Core::GraphicsFormat::R16G16_SNorm:             return DXGI_FORMAT_R16G16_SNORM;
			case Core::GraphicsFormat::R16G16B16_SNorm:          return DXGI_FORMAT_UNKNOWN;// Not supported.
			case Core::GraphicsFormat::R16G16B16A16_SNorm:       return DXGI_FORMAT_R16G16B16A16_SNORM;
			case Core::GraphicsFormat::R16_UInt:                 return DXGI_FORMAT_R16_UINT;
			case Core::GraphicsFormat::R16G16_UInt:              return DXGI_FORMAT_R16G16_UINT;
			case Core::GraphicsFormat::R16G16B16_UInt:           return DXGI_FORMAT_UNKNOWN;// Not supported.
			case Core::GraphicsFormat::R16G16B16A16_UInt:        return DXGI_FORMAT_R16G16B16A16_UINT;
			case Core::GraphicsFormat::R16_SInt:                 return DXGI_FORMAT_R16_SINT;
			case Core::GraphicsFormat::R16G16_SInt:              return DXGI_FORMAT_R16G16_SINT;
			case Core::GraphicsFormat::R16G16B16_SInt:           return DXGI_FORMAT_UNKNOWN;// Not supported.
			case Core::GraphicsFormat::R16G16B16A16_SInt:        return DXGI_FORMAT_R16G16B16A16_SINT;

			case Core::GraphicsFormat::R32_UInt:                 return DXGI_FORMAT_R32_UINT;
			case Core::GraphicsFormat::R32G32_UInt:              return DXGI_FORMAT_R32G32_UINT;
			case Core::GraphicsFormat::R32G32B32_UInt:           return DXGI_FORMAT_R32G32B32_UINT;
			case Core::GraphicsFormat::R32G32B32A32_UInt:        return DXGI_FORMAT_R32G32B32A32_UINT;
			case Core::GraphicsFormat::R32_SInt:                 return DXGI_FORMAT_R32_SINT;
			case Core::GraphicsFormat::R32G32_SInt:              return DXGI_FORMAT_R32G32_SINT;
			case Core::GraphicsFormat::R32G32B32_SInt:           return DXGI_FORMAT_R32G32B32_SINT;
			case Core::GraphicsFormat::R32G32B32A32_SInt:        return DXGI_FORMAT_R32G32B32A32_SINT;

			case Core::GraphicsFormat::R16_SFloat:               return DXGI_FORMAT_R16_FLOAT;
			case Core::GraphicsFormat::R16G16_SFloat:            return DXGI_FORMAT_R16G16_FLOAT;
			case Core::GraphicsFormat::R16G16B16_SFloat:         return DXGI_FORMAT_UNKNOWN;// Not supported.
			case Core::GraphicsFormat::R16G16B16A16_SFloat:      return DXGI_FORMAT_R16G16B16A16_FLOAT;
			case Core::GraphicsFormat::R32_SFloat:               return DXGI_FORMAT_R32_FLOAT;
			case Core::GraphicsFormat::R32G32_SFloat:            return DXGI_FORMAT_R32G32_FLOAT;
			case Core::GraphicsFormat::R32G32B32_SFloat:         return DXGI_FORMAT_R32G32B32_FLOAT;
			case Core::GraphicsFormat::R32G32B32A32_SFloat:      return DXGI_FORMAT_R32G32B32A32_FLOAT;

			case Core::GraphicsFormat::B8G8R8_UNorm:             return DXGI_FORMAT_UNKNOWN;// Not supported.
			case Core::GraphicsFormat::B8G8R8A8_UNorm:           return DXGI_FORMAT_B8G8R8A8_UNORM;
			case Core::GraphicsFormat::B8G8R8_SNorm:             return DXGI_FORMAT_UNKNOWN;// Not supported.
			case Core::GraphicsFormat::B8G8R8A8_SNorm:           return DXGI_FORMAT_UNKNOWN;// Not supported.
			case Core::GraphicsFormat::B8G8R8_UInt:              return DXGI_FORMAT_UNKNOWN;// Not supported.
			case Core::GraphicsFormat::B8G8R8A8_UInt:            return DXGI_FORMAT_UNKNOWN;// Not supported.
			case Core::GraphicsFormat::B8G8R8_SInt:              return DXGI_FORMAT_UNKNOWN;// Not supported.
			case Core::GraphicsFormat::B8G8R8A8_SInt:            return DXGI_FORMAT_UNKNOWN;// Not supported.

			case Core::GraphicsFormat::R4G4B4A4_UNormPack16:     return DXGI_FORMAT_UNKNOWN;// Not supported.
			case Core::GraphicsFormat::B4G4R4A4_UNormPack16:     return DXGI_FORMAT_UNKNOWN;// Not supported.
			case Core::GraphicsFormat::R5G6B5_UNormPack16:       return DXGI_FORMAT_UNKNOWN;// Not supported.
			case Core::GraphicsFormat::B5G6R5_UNormPack16:       return DXGI_FORMAT_UNKNOWN;// Not supported.
			case Core::GraphicsFormat::R5G5B5A1_UNormPack16:     return DXGI_FORMAT_UNKNOWN;// Not supported.
			case Core::GraphicsFormat::B5G5R5A1_UNormPack16:     return DXGI_FORMAT_UNKNOWN;// Not supported.
			case Core::GraphicsFormat::A1R5G5B5_UNormPack16:     return DXGI_FORMAT_UNKNOWN;// Not supported.

			case Core::GraphicsFormat::E5B9G9R9_UFloatPack32:    return DXGI_FORMAT_UNKNOWN;// Not supported.
			case Core::GraphicsFormat::B10G11R11_UFloatPack32:   return DXGI_FORMAT_UNKNOWN;// Not supported.

			case Core::GraphicsFormat::A2B10G10R10_UNormPack32:  return DXGI_FORMAT_UNKNOWN;// Not supported.
			case Core::GraphicsFormat::A2B10G10R10_UIntPack32:   return DXGI_FORMAT_UNKNOWN;// Not supported.
			case Core::GraphicsFormat::A2B10G10R10_SIntPack32:   return DXGI_FORMAT_UNKNOWN;// Not supported.
			case Core::GraphicsFormat::R10G10B10A2_UNormPack32:  return DXGI_FORMAT_R10G10B10A2_UNORM;
			case Core::GraphicsFormat::R10G10B10A2_UIntPack32:   return DXGI_FORMAT_R10G10B10A2_UINT;
			case Core::GraphicsFormat::R10G10B10A2_SIntPack32:   return DXGI_FORMAT_UNKNOWN;// Not supported.

			case Core::GraphicsFormat::D16_UNorm:                return DXGI_FORMAT_D16_UNORM;
			case Core::GraphicsFormat::D24_UNorm:                return DXGI_FORMAT_UNKNOWN;// Not supported.
			case Core::GraphicsFormat::D24_UNorm_S8_UInt:        return DXGI_FORMAT_D24_UNORM_S8_UINT;
			case Core::GraphicsFormat::D32_SFloat:               return DXGI_FORMAT_D32_FLOAT;
			case Core::GraphicsFormat::D32_SFloat_S8_UInt:       return DXGI_FORMAT_D32_FLOAT_S8X24_UINT;
			case Core::GraphicsFormat::S8_UInt:                  return DXGI_FORMAT_UNKNOWN;// Not supported.

			case Core::GraphicsFormat::RGBA_DXT1_SRGB:           return DXGI_FORMAT_BC1_UNORM_SRGB;
			case Core::GraphicsFormat::RGBA_DXT1_UNorm:          return DXGI_FORMAT_BC1_UNORM;
			case Core::GraphicsFormat::RGBA_DXT3_SRGB:           return DXGI_FORMAT_BC2_UNORM_SRGB;
			case Core::GraphicsFormat::RGBA_DXT3_UNorm:          return DXGI_FORMAT_BC2_UNORM;
			case Core::GraphicsFormat::RGBA_DXT5_SRGB:           return DXGI_FORMAT_BC3_UNORM_SRGB;
			case Core::GraphicsFormat::RGBA_DXT5_UNorm:          return DXGI_FORMAT_BC3_UNORM;
			case Core::GraphicsFormat::R_BC4_UNorm:              return DXGI_FORMAT_BC4_UNORM;
			case Core::GraphicsFormat::R_BC4_SNorm:              return DXGI_FORMAT_BC4_SNORM;
			case Core::GraphicsFormat::RG_BC5_UNorm:             return DXGI_FORMAT_BC5_UNORM;
			case Core::GraphicsFormat::RG_BC5_SNorm:             return DXGI_FORMAT_BC5_SNORM;
			case Core::GraphicsFormat::RGB_BC6H_UFloat:          return DXGI_FORMAT_BC6H_UF16;
			case Core::GraphicsFormat::RGB_BC6H_SFloat:          return DXGI_FORMAT_BC6H_SF16;
			case Core::GraphicsFormat::RGBA_BC7_SRGB:            return DXGI_FORMAT_BC7_UNORM_SRGB;
			case Core::GraphicsFormat::RGBA_BC7_UNorm:           return DXGI_FORMAT_BC7_UNORM;

			default: return DXGI_FORMAT_UNKNOWN;
		}
	}

	Core::GraphicsFormat ToGraphicsFormat(const Core::RenderTextureFormat& aRTFormat)
	{
		switch (aRTFormat)
		{
			case Core::RenderTextureFormat::Default:    return Core::GraphicsFormat::B8G8R8A8_UNorm;
			case Core::RenderTextureFormat::DefaultHDR: return Core::GraphicsFormat::R16G16B16_UNorm;
			case Core::RenderTextureFormat::Depth:      return Core::GraphicsFormat::None;
			case Core::RenderTextureFormat::Shadowmap:  return Core::GraphicsFormat::None;

				// One channel
			case Core::RenderTextureFormat::RInt:       return Core::GraphicsFormat::R8_SInt;
			case Core::RenderTextureFormat::RFloat:     return Core::GraphicsFormat::R32_SFloat;

			case Core::RenderTextureFormat::R8:         return Core::GraphicsFormat::R8_UInt;
			case Core::RenderTextureFormat::R16:        return Core::GraphicsFormat::R16_UInt;

				// Two channels
			case Core::RenderTextureFormat::RGInt:      return Core::GraphicsFormat::R8G8_SInt;
			case Core::RenderTextureFormat::RGFloat:    return Core::GraphicsFormat::R32G32_SFloat;

			case Core::RenderTextureFormat::RG16:       return Core::GraphicsFormat::R16G16_UInt;
			case Core::RenderTextureFormat::RG32:       return Core::GraphicsFormat::R32G32_UInt;

				// Three channels
			case Core::RenderTextureFormat::RGB565:     return Core::GraphicsFormat::R5G6B5_UNormPack16;

				// Four channels
			case Core::RenderTextureFormat::RGBAInt:    return Core::GraphicsFormat::R8G8B8A8_SInt;
			case Core::RenderTextureFormat::RGBAFloat:  return Core::GraphicsFormat::R32G32B32A32_SFloat;

			case Core::RenderTextureFormat::RGBA32:     return Core::GraphicsFormat::R32G32B32A32_UInt;

			default: return Core::GraphicsFormat::None;
		}
	}

	Core::GraphicsFormat ToGraphicsFormat(const Core::TextureFormat& aFormat)
	{
		switch (aFormat)
		{
			case Core::TextureFormat::Alpha8:              return Core::GraphicsFormat::R8_UInt;
			case Core::TextureFormat::ARGB4444:            return Core::GraphicsFormat::None;
			case Core::TextureFormat::RGB24:               return Core::GraphicsFormat::R8G8B8_UInt;
			case Core::TextureFormat::RGBA32:              return Core::GraphicsFormat::R8G8B8A8_UInt;
			case Core::TextureFormat::ARGB32:              return Core::GraphicsFormat::None;
			case Core::TextureFormat::RGB565:              return Core::GraphicsFormat::R5G6B5_UNormPack16;
			case Core::TextureFormat::R16:                 return Core::GraphicsFormat::R16_UInt;
			case Core::TextureFormat::DXT1:                return Core::GraphicsFormat::RGBA_DXT1_SRGB;
			case Core::TextureFormat::DXT3:                return Core::GraphicsFormat::RGBA_DXT3_SRGB;
			case Core::TextureFormat::DXT5:                return Core::GraphicsFormat::RGBA_DXT5_SRGB;
			case Core::TextureFormat::RGBA4444:            return Core::GraphicsFormat::B4G4R4A4_UNormPack16;
			case Core::TextureFormat::BGRA32:              return Core::GraphicsFormat::B8G8R8A8_UInt;

			case Core::TextureFormat::RHalf:               return Core::GraphicsFormat::R16_SFloat;
			case Core::TextureFormat::RGHalf:              return Core::GraphicsFormat::R16G16_SFloat;
			case Core::TextureFormat::RGBAHalf:            return Core::GraphicsFormat::R16G16B16A16_SFloat;
			case Core::TextureFormat::RFloat:              return Core::GraphicsFormat::R32_SFloat;
			case Core::TextureFormat::RGFloat:             return Core::GraphicsFormat::R32G32_SFloat;
			case Core::TextureFormat::RGBAFloat:           return Core::GraphicsFormat::R32G32B32A32_SFloat;

			case Core::TextureFormat::BC4:                 return Core::GraphicsFormat::R_BC4_UNorm;
			case Core::TextureFormat::BC5:                 return Core::GraphicsFormat::RG_BC5_UNorm;
			case Core::TextureFormat::BC6H:                return Core::GraphicsFormat::RGB_BC6H_SFloat;
			case Core::TextureFormat::BC7:                 return Core::GraphicsFormat::RGBA_BC7_SRGB;

			case Core::TextureFormat::DXT1Crunched:        return Core::GraphicsFormat::RGBA_DXT1_SRGB;
			case Core::TextureFormat::DXT5Crunched:        return Core::GraphicsFormat::RGBA_DXT5_SRGB;

			case Core::TextureFormat::RG16:                return Core::GraphicsFormat::R8G8_UInt;
			case Core::TextureFormat::R8:                  return Core::GraphicsFormat::R8_UInt;

				// 16-bit raw integer formats
			case Core::TextureFormat::RG32:                return Core::GraphicsFormat::R16G16_UInt;
			case Core::TextureFormat::RGB48:               return Core::GraphicsFormat::R16G16B16_UInt;
			case Core::TextureFormat::RGBA64:              return Core::GraphicsFormat::R16G16B16A16_UInt;
			case Core::TextureFormat::R8_SIGNED:           return Core::GraphicsFormat::R8_SInt;
			case Core::TextureFormat::RG16_SIGNED:         return Core::GraphicsFormat::R8G8_SInt;
			case Core::TextureFormat::RGB24_SIGNED:        return Core::GraphicsFormat::R8G8B8_SInt;
			case Core::TextureFormat::RGBA32_SIGNED:       return Core::GraphicsFormat::R8G8B8A8_SInt;
			case Core::TextureFormat::R16_SIGNED:          return Core::GraphicsFormat::R16_UInt;
			case Core::TextureFormat::RG32_SIGNED:         return Core::GraphicsFormat::R16G16_UInt;
			case Core::TextureFormat::RGB48_SIGNED:        return Core::GraphicsFormat::R16G16B16_UInt;
			case Core::TextureFormat::RGBA64_SIGNE2:       return Core::GraphicsFormat::R16G16B16A16_UInt;

			default: return Core::GraphicsFormat::None;
		}
	}

	D3D12_RESOURCE_DIMENSION ToD3DTextureDimension(const Core::TextureDimension& aDimension)
	{
		switch (aDimension)
		{
			case Core::TextureDimension::None:
				return D3D12_RESOURCE_DIMENSION_BUFFER;
			case Core::TextureDimension::Cube:
			case Core::TextureDimension::CubeArray:
			case Core::TextureDimension::Tex2D:
				return D3D12_RESOURCE_DIMENSION_TEXTURE2D;
			case Core::TextureDimension::Tex3D:
				return D3D12_RESOURCE_DIMENSION_TEXTURE3D;
			default:
				return D3D12_RESOURCE_DIMENSION_UNKNOWN;
		}
	}

	D3D12_RTV_DIMENSION ToRTVTextureDimension(const Core::TextureDimension& aDimension)
	{
		switch (aDimension)
		{
			case Core::TextureDimension::None:
				return D3D12_RTV_DIMENSION_BUFFER;
			case Core::TextureDimension::Tex2D:
				return D3D12_RTV_DIMENSION_TEXTURE2D;
			case Core::TextureDimension::Tex3D:
				return D3D12_RTV_DIMENSION_TEXTURE3D;
			default:
				return D3D12_RTV_DIMENSION_UNKNOWN;
		}
	}

	D3D12_DSV_DIMENSION ToDSVTextureDimension(const Core::TextureDimension& aDimension)
	{
		switch (aDimension)
		{
			case Core::TextureDimension::Tex2D:
				return D3D12_DSV_DIMENSION_TEXTURE2D;
			default:
				return D3D12_DSV_DIMENSION_UNKNOWN;
		}
	}

	Core::TextureFormat ToTextureFormat(const DXGI_FORMAT& aFormat)
	{
		switch (aFormat)
		{
			case DXGI_FORMAT_BC1_TYPELESS:
			case DXGI_FORMAT_BC1_UNORM:
			case DXGI_FORMAT_BC1_UNORM_SRGB:
				return Core::TextureFormat::DXT1;

			case DXGI_FORMAT_BC2_TYPELESS:
			case DXGI_FORMAT_BC2_UNORM:
			case DXGI_FORMAT_BC2_UNORM_SRGB:
				return Core::TextureFormat::DXT3;

			case DXGI_FORMAT_BC3_TYPELESS:
			case DXGI_FORMAT_BC3_UNORM:
			case DXGI_FORMAT_BC3_UNORM_SRGB:
				return Core::TextureFormat::DXT5;

			case DXGI_FORMAT_BC4_TYPELESS:
			case DXGI_FORMAT_BC4_UNORM:
			case DXGI_FORMAT_BC4_SNORM:
				return Core::TextureFormat::BC4;

			case DXGI_FORMAT_BC5_TYPELESS:
			case DXGI_FORMAT_BC5_UNORM:
			case DXGI_FORMAT_BC5_SNORM:
				return Core::TextureFormat::BC5;

			case DXGI_FORMAT_BC6H_TYPELESS:
			case DXGI_FORMAT_BC6H_UF16:
			case DXGI_FORMAT_BC6H_SF16:
				return Core::TextureFormat::BC6H;

			case DXGI_FORMAT_BC7_TYPELESS:
			case DXGI_FORMAT_BC7_UNORM:
			case DXGI_FORMAT_BC7_UNORM_SRGB:
				return Core::TextureFormat::BC7;

			default:
				Debug::LogFatal("Unsupported format.");
				return Core::TextureFormat(0);
		}
	}
}
