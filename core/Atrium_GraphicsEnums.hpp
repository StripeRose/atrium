// Filter "Graphics"

#pragma once

namespace Atrium
{
	enum class TextureCubeFace
	{
		PositiveX,
		NegativeX,
		PositiveY,
		NegativeY,
		PositiveZ,
		NegativeZ,
	};

	enum class TextureDimension
	{
		// Uninitialized or unknown
		Unknown,
		// No texture assigned
		None,
		// Any texture type
		Any,
		// 2D texture (Texture2D)
		Tex2D,
		// 3D texture (Texture3D)
		Tex3D,
		// Cubemap texture (TextureCube)
		Cube,
		// Cubemap array texture (TextureCubeArray)
		CubeArray
	};

	enum class FilterMode
	{
		// Point filtering - Texture pixels become blocky up close
		Point,
		// Bilinear filtering - Texture samples are averaged
		Bilinear,
		// Trilinear filtering - Texture samples are averaged and also blended between the mipmap levels
		Trilinear
	};

	/**
	 * @brief Formats for graphics-buffers in CPU or GPU memory.
	 */
	enum class GraphicsFormat
	{
		None,

		R8_UNorm,
		R8G8_UNorm,
		R8G8B8_UNorm,
		R8G8B8A8_UNorm,
		R8_SNorm,
		R8G8_SNorm,
		R8G8B8_SNorm,
		R8G8B8A8_SNorm,
		R8_UInt,
		R8G8_UInt,
		R8G8B8_UInt,
		R8G8B8A8_UInt,
		R8_SInt,
		R8G8_SInt,
		R8G8B8_SInt,
		R8G8B8A8_SInt,

		R16_UNorm,
		R16G16_UNorm,
		R16G16B16_UNorm,
		R16G16B16A16_UNorm,
		R16_SNorm,
		R16G16_SNorm,
		R16G16B16_SNorm,
		R16G16B16A16_SNorm,
		R16_UInt,
		R16G16_UInt,
		R16G16B16_UInt,
		R16G16B16A16_UInt,
		R16_SInt,
		R16G16_SInt,
		R16G16B16_SInt,
		R16G16B16A16_SInt,

		R32_UInt,
		R32G32_UInt,
		R32G32B32_UInt,
		R32G32B32A32_UInt,
		R32_SInt,
		R32G32_SInt,
		R32G32B32_SInt,
		R32G32B32A32_SInt,

		R16_SFloat,
		R16G16_SFloat,
		R16G16B16_SFloat,
		R16G16B16A16_SFloat,
		R32_SFloat,
		R32G32_SFloat,
		R32G32B32_SFloat,
		R32G32B32A32_SFloat,

		B8G8R8_UNorm,
		B8G8R8A8_UNorm,
		B8G8R8_SNorm,
		B8G8R8A8_SNorm,
		B8G8R8_UInt,
		B8G8R8A8_UInt,
		B8G8R8_SInt,
		B8G8R8A8_SInt,

		R4G4B4A4_UNormPack16,
		B4G4R4A4_UNormPack16,
		R5G6B5_UNormPack16,
		B5G6R5_UNormPack16,
		R5G5B5A1_UNormPack16,
		B5G5R5A1_UNormPack16,
		A1R5G5B5_UNormPack16,

		E5B9G9R9_UFloatPack32,
		B10G11R11_UFloatPack32,

		A2B10G10R10_UNormPack32,
		A2B10G10R10_UIntPack32,
		A2B10G10R10_SIntPack32,
		R10G10B10A2_UNormPack32,
		R10G10B10A2_UIntPack32,
		R10G10B10A2_SIntPack32,

		D16_UNorm,
		D24_UNorm,
		D24_UNorm_S8_UInt,
		D32_SFloat,
		D32_SFloat_S8_UInt,
		S8_UInt,

		RGBA_DXT1_SRGB,
		RGBA_DXT1_UNorm,
		RGBA_DXT3_SRGB,
		RGBA_DXT3_UNorm,
		RGBA_DXT5_SRGB,
		RGBA_DXT5_UNorm,
		R_BC4_UNorm,
		R_BC4_SNorm,
		RG_BC5_UNorm,
		RG_BC5_SNorm,
		RGB_BC6H_UFloat,
		RGB_BC6H_SFloat,
		RGBA_BC7_SRGB,
		RGBA_BC7_UNorm
	};

	enum class FormatUsage
	{
		Sample
	};

	enum class PrimitiveTopology
	{
		PointList,
		LineList,
		LineStrip,
		TriangleList,
		TriangleStrip
	};

	enum class TextureWrapMode
	{
		Repeat,
		Clamp,
		Mirror,
		MirrorOnce
	};

	/**
	 * @brief Formats for textures on disk.
	 */
	enum class TextureFormat
	{
		Alpha8,
		ARGB4444,
		RGB24,
		RGBA32,
		ARGB32,
		RGB565,
		R16,
		DXT1,
		DXT3,
		DXT5,
		RGBA4444,
		BGRA32,

		RHalf,
		RGHalf,
		RGBAHalf,
		RFloat,
		RGFloat,
		RGBAFloat,

		BC4,
		BC5,
		BC6H,
		BC7,

		DXT1Crunched,
		DXT5Crunched,

		RG16,
		R8,

		// 16-bit raw integer formats
		RG32,
		RGB48,
		RGBA64,
		R8_SIGNED,
		RG16_SIGNED,
		RGB24_SIGNED,
		RGBA32_SIGNED,
		R16_SIGNED,
		RG32_SIGNED,
		RGB48_SIGNED,
		RGBA64_SIGNE2
	};

	/**
	 * @brief Commonly used render-texture purposes.
	 *        The active Graphics API will resolve this to the appropriate
	 *        color- and depth-buffer formats, unless overridden.
	 */
	enum class RenderTextureFormat
	{
		Default,
		DefaultHDR,

		Depth,

		Shadowmap,

		// One channel
		RInt,
		RFloat,
		R8,
		R16,

		// Two channels
		RGInt,
		RGFloat,

		RG16,
		RG32,

		// Three channels
		RGB565,

		// Four channels
		RGBAInt,
		RGBAFloat,
		RGBA32,
	};
}