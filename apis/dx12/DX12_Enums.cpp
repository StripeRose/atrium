#include "DX12_Diagnostics.hpp"
#include "DX12_Enums.hpp"

namespace Atrium::DirectX12
{
	DXGI_FORMAT ToDXGIFormat(const GraphicsFormat& aFormat)
	{
		switch (aFormat)
		{
			case GraphicsFormat::None:                     return DXGI_FORMAT_UNKNOWN;

			case GraphicsFormat::R8_UNorm:                 return DXGI_FORMAT_R8_UNORM;
			case GraphicsFormat::R8G8_UNorm:               return DXGI_FORMAT_R8G8_UNORM;
			case GraphicsFormat::R8G8B8_UNorm:             return DXGI_FORMAT_R8G8B8A8_UNORM;
			case GraphicsFormat::R8G8B8A8_UNorm:           return DXGI_FORMAT_R8G8B8A8_UNORM;
			case GraphicsFormat::R8_SNorm:                 return DXGI_FORMAT_R8_SNORM;
			case GraphicsFormat::R8G8_SNorm:               return DXGI_FORMAT_R8G8_SNORM;
			case GraphicsFormat::R8G8B8_SNorm:             return DXGI_FORMAT_UNKNOWN;// Not supported.
			case GraphicsFormat::R8G8B8A8_SNorm:           return DXGI_FORMAT_R8G8B8A8_SNORM;
			case GraphicsFormat::R8_UInt:                  return DXGI_FORMAT_R8_UINT;
			case GraphicsFormat::R8G8_UInt:                return DXGI_FORMAT_R8G8_UINT;
			case GraphicsFormat::R8G8B8_UInt:              return DXGI_FORMAT_UNKNOWN;// Not supported.
			case GraphicsFormat::R8G8B8A8_UInt:            return DXGI_FORMAT_R8G8B8A8_UINT;
			case GraphicsFormat::R8_SInt:                  return DXGI_FORMAT_R8_SINT;
			case GraphicsFormat::R8G8_SInt:                return DXGI_FORMAT_R8G8_SINT;
			case GraphicsFormat::R8G8B8_SInt:              return DXGI_FORMAT_UNKNOWN;// Not supported.
			case GraphicsFormat::R8G8B8A8_SInt:            return DXGI_FORMAT_R8G8B8A8_SINT;

			case GraphicsFormat::R16_UNorm:                return DXGI_FORMAT_R16_UNORM;
			case GraphicsFormat::R16G16_UNorm:             return DXGI_FORMAT_R16G16_UNORM;
			case GraphicsFormat::R16G16B16_UNorm:          return DXGI_FORMAT_UNKNOWN;// Not supported.
			case GraphicsFormat::R16G16B16A16_UNorm:       return DXGI_FORMAT_R16G16B16A16_UNORM;
			case GraphicsFormat::R16_SNorm:                return DXGI_FORMAT_R16_SNORM;
			case GraphicsFormat::R16G16_SNorm:             return DXGI_FORMAT_R16G16_SNORM;
			case GraphicsFormat::R16G16B16_SNorm:          return DXGI_FORMAT_UNKNOWN;// Not supported.
			case GraphicsFormat::R16G16B16A16_SNorm:       return DXGI_FORMAT_R16G16B16A16_SNORM;
			case GraphicsFormat::R16_UInt:                 return DXGI_FORMAT_R16_UINT;
			case GraphicsFormat::R16G16_UInt:              return DXGI_FORMAT_R16G16_UINT;
			case GraphicsFormat::R16G16B16_UInt:           return DXGI_FORMAT_UNKNOWN;// Not supported.
			case GraphicsFormat::R16G16B16A16_UInt:        return DXGI_FORMAT_R16G16B16A16_UINT;
			case GraphicsFormat::R16_SInt:                 return DXGI_FORMAT_R16_SINT;
			case GraphicsFormat::R16G16_SInt:              return DXGI_FORMAT_R16G16_SINT;
			case GraphicsFormat::R16G16B16_SInt:           return DXGI_FORMAT_UNKNOWN;// Not supported.
			case GraphicsFormat::R16G16B16A16_SInt:        return DXGI_FORMAT_R16G16B16A16_SINT;

			case GraphicsFormat::R32_UInt:                 return DXGI_FORMAT_R32_UINT;
			case GraphicsFormat::R32G32_UInt:              return DXGI_FORMAT_R32G32_UINT;
			case GraphicsFormat::R32G32B32_UInt:           return DXGI_FORMAT_R32G32B32_UINT;
			case GraphicsFormat::R32G32B32A32_UInt:        return DXGI_FORMAT_R32G32B32A32_UINT;
			case GraphicsFormat::R32_SInt:                 return DXGI_FORMAT_R32_SINT;
			case GraphicsFormat::R32G32_SInt:              return DXGI_FORMAT_R32G32_SINT;
			case GraphicsFormat::R32G32B32_SInt:           return DXGI_FORMAT_R32G32B32_SINT;
			case GraphicsFormat::R32G32B32A32_SInt:        return DXGI_FORMAT_R32G32B32A32_SINT;

			case GraphicsFormat::R16_SFloat:               return DXGI_FORMAT_R16_FLOAT;
			case GraphicsFormat::R16G16_SFloat:            return DXGI_FORMAT_R16G16_FLOAT;
			case GraphicsFormat::R16G16B16_SFloat:         return DXGI_FORMAT_UNKNOWN;// Not supported.
			case GraphicsFormat::R16G16B16A16_SFloat:      return DXGI_FORMAT_R16G16B16A16_FLOAT;
			case GraphicsFormat::R32_SFloat:               return DXGI_FORMAT_R32_FLOAT;
			case GraphicsFormat::R32G32_SFloat:            return DXGI_FORMAT_R32G32_FLOAT;
			case GraphicsFormat::R32G32B32_SFloat:         return DXGI_FORMAT_R32G32B32_FLOAT;
			case GraphicsFormat::R32G32B32A32_SFloat:      return DXGI_FORMAT_R32G32B32A32_FLOAT;

			case GraphicsFormat::B8G8R8_UNorm:             return DXGI_FORMAT_UNKNOWN;// Not supported.
			case GraphicsFormat::B8G8R8A8_UNorm:           return DXGI_FORMAT_B8G8R8A8_UNORM;
			case GraphicsFormat::B8G8R8_SNorm:             return DXGI_FORMAT_UNKNOWN;// Not supported.
			case GraphicsFormat::B8G8R8A8_SNorm:           return DXGI_FORMAT_UNKNOWN;// Not supported.
			case GraphicsFormat::B8G8R8_UInt:              return DXGI_FORMAT_UNKNOWN;// Not supported.
			case GraphicsFormat::B8G8R8A8_UInt:            return DXGI_FORMAT_UNKNOWN;// Not supported.
			case GraphicsFormat::B8G8R8_SInt:              return DXGI_FORMAT_UNKNOWN;// Not supported.
			case GraphicsFormat::B8G8R8A8_SInt:            return DXGI_FORMAT_UNKNOWN;// Not supported.

			case GraphicsFormat::R4G4B4A4_UNormPack16:     return DXGI_FORMAT_UNKNOWN;// Not supported.
			case GraphicsFormat::B4G4R4A4_UNormPack16:     return DXGI_FORMAT_UNKNOWN;// Not supported.
			case GraphicsFormat::R5G6B5_UNormPack16:       return DXGI_FORMAT_UNKNOWN;// Not supported.
			case GraphicsFormat::B5G6R5_UNormPack16:       return DXGI_FORMAT_UNKNOWN;// Not supported.
			case GraphicsFormat::R5G5B5A1_UNormPack16:     return DXGI_FORMAT_UNKNOWN;// Not supported.
			case GraphicsFormat::B5G5R5A1_UNormPack16:     return DXGI_FORMAT_UNKNOWN;// Not supported.
			case GraphicsFormat::A1R5G5B5_UNormPack16:     return DXGI_FORMAT_UNKNOWN;// Not supported.

			case GraphicsFormat::E5B9G9R9_UFloatPack32:    return DXGI_FORMAT_UNKNOWN;// Not supported.
			case GraphicsFormat::B10G11R11_UFloatPack32:   return DXGI_FORMAT_UNKNOWN;// Not supported.

			case GraphicsFormat::A2B10G10R10_UNormPack32:  return DXGI_FORMAT_UNKNOWN;// Not supported.
			case GraphicsFormat::A2B10G10R10_UIntPack32:   return DXGI_FORMAT_UNKNOWN;// Not supported.
			case GraphicsFormat::A2B10G10R10_SIntPack32:   return DXGI_FORMAT_UNKNOWN;// Not supported.
			case GraphicsFormat::R10G10B10A2_UNormPack32:  return DXGI_FORMAT_R10G10B10A2_UNORM;
			case GraphicsFormat::R10G10B10A2_UIntPack32:   return DXGI_FORMAT_R10G10B10A2_UINT;
			case GraphicsFormat::R10G10B10A2_SIntPack32:   return DXGI_FORMAT_UNKNOWN;// Not supported.

			case GraphicsFormat::D16_UNorm:                return DXGI_FORMAT_D16_UNORM;
			case GraphicsFormat::D24_UNorm:                return DXGI_FORMAT_UNKNOWN;// Not supported.
			case GraphicsFormat::D24_UNorm_S8_UInt:        return DXGI_FORMAT_D24_UNORM_S8_UINT;
			case GraphicsFormat::D32_SFloat:               return DXGI_FORMAT_D32_FLOAT;
			case GraphicsFormat::D32_SFloat_S8_UInt:       return DXGI_FORMAT_D32_FLOAT_S8X24_UINT;
			case GraphicsFormat::S8_UInt:                  return DXGI_FORMAT_UNKNOWN;// Not supported.

			case GraphicsFormat::RGBA_DXT1_SRGB:           return DXGI_FORMAT_BC1_UNORM_SRGB;
			case GraphicsFormat::RGBA_DXT1_UNorm:          return DXGI_FORMAT_BC1_UNORM;
			case GraphicsFormat::RGBA_DXT3_SRGB:           return DXGI_FORMAT_BC2_UNORM_SRGB;
			case GraphicsFormat::RGBA_DXT3_UNorm:          return DXGI_FORMAT_BC2_UNORM;
			case GraphicsFormat::RGBA_DXT5_SRGB:           return DXGI_FORMAT_BC3_UNORM_SRGB;
			case GraphicsFormat::RGBA_DXT5_UNorm:          return DXGI_FORMAT_BC3_UNORM;
			case GraphicsFormat::R_BC4_UNorm:              return DXGI_FORMAT_BC4_UNORM;
			case GraphicsFormat::R_BC4_SNorm:              return DXGI_FORMAT_BC4_SNORM;
			case GraphicsFormat::RG_BC5_UNorm:             return DXGI_FORMAT_BC5_UNORM;
			case GraphicsFormat::RG_BC5_SNorm:             return DXGI_FORMAT_BC5_SNORM;
			case GraphicsFormat::RGB_BC6H_UFloat:          return DXGI_FORMAT_BC6H_UF16;
			case GraphicsFormat::RGB_BC6H_SFloat:          return DXGI_FORMAT_BC6H_SF16;
			case GraphicsFormat::RGBA_BC7_SRGB:            return DXGI_FORMAT_BC7_UNORM_SRGB;
			case GraphicsFormat::RGBA_BC7_UNorm:           return DXGI_FORMAT_BC7_UNORM;

			default: return DXGI_FORMAT_UNKNOWN;
		}
	}

	GraphicsFormat ToGraphicsFormat(const RenderTextureFormat& aRTFormat)
	{
		switch (aRTFormat)
		{
			case RenderTextureFormat::Default:    return GraphicsFormat::B8G8R8A8_UNorm;
			case RenderTextureFormat::DefaultHDR: return GraphicsFormat::R16G16B16_UNorm;
			case RenderTextureFormat::Depth:      return GraphicsFormat::None;
			case RenderTextureFormat::Shadowmap:  return GraphicsFormat::None;

				// One channel
			case RenderTextureFormat::RInt:       return GraphicsFormat::R8_SInt;
			case RenderTextureFormat::RFloat:     return GraphicsFormat::R32_SFloat;

			case RenderTextureFormat::R8:         return GraphicsFormat::R8_UInt;
			case RenderTextureFormat::R16:        return GraphicsFormat::R16_UInt;

				// Two channels
			case RenderTextureFormat::RGInt:      return GraphicsFormat::R8G8_SInt;
			case RenderTextureFormat::RGFloat:    return GraphicsFormat::R32G32_SFloat;

			case RenderTextureFormat::RG16:       return GraphicsFormat::R16G16_UInt;
			case RenderTextureFormat::RG32:       return GraphicsFormat::R32G32_UInt;

				// Three channels
			case RenderTextureFormat::RGB565:     return GraphicsFormat::R5G6B5_UNormPack16;

				// Four channels
			case RenderTextureFormat::RGBAInt:    return GraphicsFormat::R8G8B8A8_SInt;
			case RenderTextureFormat::RGBAFloat:  return GraphicsFormat::R32G32B32A32_SFloat;

			case RenderTextureFormat::RGBA32:     return GraphicsFormat::R32G32B32A32_UInt;

			default: return GraphicsFormat::None;
		}
	}

	GraphicsFormat ToGraphicsFormat(const TextureFormat& aFormat)
	{
		switch (aFormat)
		{
			case TextureFormat::Alpha8:              return GraphicsFormat::R8_UInt;
			case TextureFormat::ARGB4444:            return GraphicsFormat::None;
			case TextureFormat::RGB24:               return GraphicsFormat::R8G8B8_UInt;
			case TextureFormat::RGBA32:              return GraphicsFormat::R8G8B8A8_UInt;
			case TextureFormat::ARGB32:              return GraphicsFormat::None;
			case TextureFormat::RGB565:              return GraphicsFormat::R5G6B5_UNormPack16;
			case TextureFormat::R16:                 return GraphicsFormat::R16_UInt;
			case TextureFormat::DXT1:                return GraphicsFormat::RGBA_DXT1_SRGB;
			case TextureFormat::DXT3:                return GraphicsFormat::RGBA_DXT3_SRGB;
			case TextureFormat::DXT5:                return GraphicsFormat::RGBA_DXT5_SRGB;
			case TextureFormat::RGBA4444:            return GraphicsFormat::B4G4R4A4_UNormPack16;
			case TextureFormat::BGRA32:              return GraphicsFormat::B8G8R8A8_UInt;

			case TextureFormat::RHalf:               return GraphicsFormat::R16_SFloat;
			case TextureFormat::RGHalf:              return GraphicsFormat::R16G16_SFloat;
			case TextureFormat::RGBAHalf:            return GraphicsFormat::R16G16B16A16_SFloat;
			case TextureFormat::RFloat:              return GraphicsFormat::R32_SFloat;
			case TextureFormat::RGFloat:             return GraphicsFormat::R32G32_SFloat;
			case TextureFormat::RGBAFloat:           return GraphicsFormat::R32G32B32A32_SFloat;

			case TextureFormat::BC4:                 return GraphicsFormat::R_BC4_UNorm;
			case TextureFormat::BC5:                 return GraphicsFormat::RG_BC5_UNorm;
			case TextureFormat::BC6H:                return GraphicsFormat::RGB_BC6H_SFloat;
			case TextureFormat::BC7:                 return GraphicsFormat::RGBA_BC7_SRGB;

			case TextureFormat::DXT1Crunched:        return GraphicsFormat::RGBA_DXT1_SRGB;
			case TextureFormat::DXT5Crunched:        return GraphicsFormat::RGBA_DXT5_SRGB;

			case TextureFormat::RG16:                return GraphicsFormat::R8G8_UInt;
			case TextureFormat::R8:                  return GraphicsFormat::R8_UInt;

				// 16-bit raw integer formats
			case TextureFormat::RG32:                return GraphicsFormat::R16G16_UInt;
			case TextureFormat::RGB48:               return GraphicsFormat::R16G16B16_UInt;
			case TextureFormat::RGBA64:              return GraphicsFormat::R16G16B16A16_UInt;
			case TextureFormat::R8_SIGNED:           return GraphicsFormat::R8_SInt;
			case TextureFormat::RG16_SIGNED:         return GraphicsFormat::R8G8_SInt;
			case TextureFormat::RGB24_SIGNED:        return GraphicsFormat::R8G8B8_SInt;
			case TextureFormat::RGBA32_SIGNED:       return GraphicsFormat::R8G8B8A8_SInt;
			case TextureFormat::R16_SIGNED:          return GraphicsFormat::R16_UInt;
			case TextureFormat::RG32_SIGNED:         return GraphicsFormat::R16G16_UInt;
			case TextureFormat::RGB48_SIGNED:        return GraphicsFormat::R16G16B16_UInt;
			case TextureFormat::RGBA64_SIGNE2:       return GraphicsFormat::R16G16B16A16_UInt;

			default: return GraphicsFormat::None;
		}
	}

	D3D12_RESOURCE_DIMENSION ToD3DTextureDimension(const TextureDimension& aDimension)
	{
		switch (aDimension)
		{
			case TextureDimension::None:
				return D3D12_RESOURCE_DIMENSION_BUFFER;
			case TextureDimension::Cube:
			case TextureDimension::CubeArray:
			case TextureDimension::Tex2D:
				return D3D12_RESOURCE_DIMENSION_TEXTURE2D;
			case TextureDimension::Tex3D:
				return D3D12_RESOURCE_DIMENSION_TEXTURE3D;
			default:
				return D3D12_RESOURCE_DIMENSION_UNKNOWN;
		}
	}

	D3D12_RTV_DIMENSION ToRTVTextureDimension(const TextureDimension& aDimension)
	{
		switch (aDimension)
		{
			case TextureDimension::None:
				return D3D12_RTV_DIMENSION_BUFFER;
			case TextureDimension::Tex2D:
				return D3D12_RTV_DIMENSION_TEXTURE2D;
			case TextureDimension::Tex3D:
				return D3D12_RTV_DIMENSION_TEXTURE3D;
			default:
				return D3D12_RTV_DIMENSION_UNKNOWN;
		}
	}

	D3D12_DSV_DIMENSION ToDSVTextureDimension(const TextureDimension& aDimension)
	{
		switch (aDimension)
		{
			case TextureDimension::Tex2D:
				return D3D12_DSV_DIMENSION_TEXTURE2D;
			default:
				return D3D12_DSV_DIMENSION_UNKNOWN;
		}
	}

	TextureFormat ToTextureFormat(const DXGI_FORMAT& aFormat)
	{
		switch (aFormat)
		{
			case DXGI_FORMAT_BC1_TYPELESS:
			case DXGI_FORMAT_BC1_UNORM:
			case DXGI_FORMAT_BC1_UNORM_SRGB:
				return TextureFormat::DXT1;

			case DXGI_FORMAT_BC2_TYPELESS:
			case DXGI_FORMAT_BC2_UNORM:
			case DXGI_FORMAT_BC2_UNORM_SRGB:
				return TextureFormat::DXT3;

			case DXGI_FORMAT_BC3_TYPELESS:
			case DXGI_FORMAT_BC3_UNORM:
			case DXGI_FORMAT_BC3_UNORM_SRGB:
				return TextureFormat::DXT5;

			case DXGI_FORMAT_BC4_TYPELESS:
			case DXGI_FORMAT_BC4_UNORM:
			case DXGI_FORMAT_BC4_SNORM:
				return TextureFormat::BC4;

			case DXGI_FORMAT_BC5_TYPELESS:
			case DXGI_FORMAT_BC5_UNORM:
			case DXGI_FORMAT_BC5_SNORM:
				return TextureFormat::BC5;

			case DXGI_FORMAT_BC6H_TYPELESS:
			case DXGI_FORMAT_BC6H_UF16:
			case DXGI_FORMAT_BC6H_SF16:
				return TextureFormat::BC6H;

			case DXGI_FORMAT_BC7_TYPELESS:
			case DXGI_FORMAT_BC7_UNORM:
			case DXGI_FORMAT_BC7_UNORM_SRGB:
				return TextureFormat::BC7;

			default:
				Debug::LogFatal("Unsupported format.");
				return TextureFormat(0);
		}
	}
}
