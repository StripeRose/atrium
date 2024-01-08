// Filter "Resources"

#include "DX12_RenderTexture.hpp"

#include "DX12_Device.hpp"
#include "DX12_Diagnostics.hpp"
#include "DX12_Enums.hpp"

namespace RoseGold::DirectX12
{
	RenderTexture::RenderTexture(Device& aDevice, const Core::Graphics::RenderTextureDescriptor& aDescriptor)
		: RenderTexture(aDevice, aDescriptor, nullptr, nullptr)
	{ }

	RenderTexture::RenderTexture(Device& aDevice, const Core::Graphics::RenderTextureDescriptor& aDescriptor, ComPtr<ID3D12Resource> aColorBuffer, ComPtr<ID3D12Resource> aDepthBuffer)
		: myDescriptor(aDescriptor)
		, myDevicePtr(&aDevice)
		, myDepthBuffer(aDepthBuffer)
	{
		myResource = aColorBuffer;
		myUsageState = D3D12_RESOURCE_STATE_RENDER_TARGET;

		Debug::Assert(
			(myDescriptor.Size_Width * myDescriptor.Size_Height * myDescriptor.Size_Depth) > 0,
			"The render-texture has a size."
		);

		if (myDescriptor.ColorGraphicsFormat == Core::Graphics::GraphicsFormat::None)
			myDescriptor.ColorGraphicsFormat = ToGraphicsFormat(myDescriptor.ColorFormat);

		if (myDescriptor.ColorGraphicsFormat != Core::Graphics::GraphicsFormat::None)
		{
			if (!aColorBuffer)
			{
				D3D12_RESOURCE_DESC colorBufferDesc = { };
				colorBufferDesc.Dimension = ToD3DTextureDimension(myDescriptor.Dimension);
				colorBufferDesc.Format = ToDXGIFormat(myDescriptor.ColorGraphicsFormat);
				colorBufferDesc.MipLevels = 1;
				colorBufferDesc.SampleDesc.Count = 1;
				colorBufferDesc.SampleDesc.Quality = 0;

				colorBufferDesc.Width = myDescriptor.Size_Width;
				colorBufferDesc.Height = myDescriptor.Size_Height;
				colorBufferDesc.DepthOrArraySize = static_cast<UINT16>(myDescriptor.Size_Depth);

				colorBufferDesc.Flags |= D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;

				D3D12_HEAP_PROPERTIES defaultHeapProperties;
				defaultHeapProperties.Type = D3D12_HEAP_TYPE_DEFAULT;
				defaultHeapProperties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
				defaultHeapProperties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
				defaultHeapProperties.CreationNodeMask = 0;
				defaultHeapProperties.VisibleNodeMask = 0;

				D3D12_CLEAR_VALUE clearValue = {};
				clearValue.Format = colorBufferDesc.Format;
				clearValue.Color[0] = 0.1f;
				clearValue.Color[1] = 0.f;
				clearValue.Color[2] = 0.1f;
				clearValue.Color[3] = 1.f;

				AssertAction(aDevice.GetDevice()->CreateCommittedResource(
					&defaultHeapProperties,
					D3D12_HEAP_FLAG_NONE,
					&colorBufferDesc,
					myUsageState,
					&clearValue,
					IID_PPV_ARGS(myResource.ReleaseAndGetAddressOf())
				), "Create render texture color buffer.");

				myResource->SetName(L"RenderTexture Depth");
			}

			D3D12_RENDER_TARGET_VIEW_DESC rtvDesc = { };
			rtvDesc.Format = ToDXGIFormat(myDescriptor.ColorGraphicsFormat);
			rtvDesc.ViewDimension = ToRTVTextureDimension(myDescriptor.Dimension);

			myRSVHandle = aDevice.GetDescriptorHeapManager().GetRTVHeap().GetNewHeapHandle();
			aDevice.GetDevice()->CreateRenderTargetView(myResource.Get(), &rtvDesc, myRSVHandle->GetCPUHandle());
		}

		if (myDescriptor.DepthStencilFormat != Core::Graphics::GraphicsFormat::None)
		{
			if (!myDepthBuffer)
			{
				D3D12_RESOURCE_DESC depthBufferDesc = { };
				depthBufferDesc.Dimension = ToD3DTextureDimension(myDescriptor.Dimension);
				depthBufferDesc.Format = ToDXGIFormat(myDescriptor.DepthStencilFormat);
				depthBufferDesc.MipLevels = 1;
				depthBufferDesc.SampleDesc.Count = 1;
				depthBufferDesc.SampleDesc.Quality = 0;

				depthBufferDesc.Width = myDescriptor.Size_Width;
				depthBufferDesc.Height = myDescriptor.Size_Height;
				depthBufferDesc.DepthOrArraySize = static_cast<UINT16>(myDescriptor.Size_Depth);

				depthBufferDesc.Flags |= D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

				D3D12_HEAP_PROPERTIES defaultHeapProperties;
				defaultHeapProperties.Type = D3D12_HEAP_TYPE_DEFAULT;
				defaultHeapProperties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
				defaultHeapProperties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
				defaultHeapProperties.CreationNodeMask = 0;
				defaultHeapProperties.VisibleNodeMask = 0;

				D3D12_CLEAR_VALUE clearValue = {};
				clearValue.Format = depthBufferDesc.Format;
				clearValue.DepthStencil.Depth = 1.f; // Farthest distance.
				clearValue.DepthStencil.Stencil = 0u;

				AssertAction(aDevice.GetDevice()->CreateCommittedResource(
					&defaultHeapProperties,
					D3D12_HEAP_FLAG_NONE,
					&depthBufferDesc,
					D3D12_RESOURCE_STATE_DEPTH_WRITE,
					&clearValue,
					IID_PPV_ARGS(myDepthBuffer.ReleaseAndGetAddressOf())
				), "Create render texture depth buffer.");

				myDepthBuffer->SetName(L"RenderTexture Depth");
			}

			D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
			dsvDesc.Format = ToDXGIFormat(myDescriptor.DepthStencilFormat);
			dsvDesc.ViewDimension = ToDSVTextureDimension(myDescriptor.Dimension);

			myDSVHandle = aDevice.GetDescriptorHeapManager().GetDSVHeap().GetNewHeapHandle();
			aDevice.GetDevice()->CreateDepthStencilView(myDepthBuffer.Get(), &dsvDesc, myDSVHandle->GetCPUHandle());
		}
	}

	Core::Graphics::TextureDimension RenderTexture::GetDimensions() const
	{
		return myDescriptor.Dimension;
	}

	unsigned int RenderTexture::GetDepth() const
	{
		return myDescriptor.Size_Depth;
	}

	void RenderTexture::SetFilterMode(Core::Graphics::FilterMode aFilterMode)
	{
		aFilterMode;
	}

	Core::Graphics::FilterMode RenderTexture::GetFilterMode() const
	{
		return Core::Graphics::FilterMode::Bilinear;
	}

	unsigned int RenderTexture::GetHeight() const
	{
		return myDescriptor.Size_Height;
	}

	bool RenderTexture::IsReadable() const
	{
		return false;
	}

	unsigned int RenderTexture::GetMipmapCount() const
	{
		return 1;
	}

	unsigned int RenderTexture::GetWidth() const
	{
		return myDescriptor.Size_Width;
	}

	Core::Graphics::TextureWrapMode RenderTexture::GetWrapModeU() const
	{
		return Core::Graphics::TextureWrapMode::Repeat;
	}

	Core::Graphics::TextureWrapMode RenderTexture::GetWrapModeV() const
	{
		return Core::Graphics::TextureWrapMode::Repeat;
	}

	Core::Graphics::TextureWrapMode RenderTexture::GetWrapModeW() const
	{
		return Core::Graphics::TextureWrapMode::Repeat;
	}

	void RenderTexture::SetWrapMode(Core::Graphics::TextureWrapMode aWrapMode)
	{
		aWrapMode;
	}

	void RenderTexture::SetWrapModeU(Core::Graphics::TextureWrapMode aWrapMode) const
	{
		aWrapMode;
	}

	void RenderTexture::SetWrapModeV(Core::Graphics::TextureWrapMode aWrapMode) const
	{
		aWrapMode;
	}

	void RenderTexture::SetWrapModeW(Core::Graphics::TextureWrapMode aWrapMode) const
	{
		aWrapMode;
	}
}
