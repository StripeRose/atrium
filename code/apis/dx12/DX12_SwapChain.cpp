// Filter "Resources"

#include "stdafx.hpp"

#include "Common_Math.hpp"

#include "DX12_CommandQueue.hpp"
#include "DX12_Device.hpp"
#include "DX12_Diagnostics.hpp"
#include "DX12_SwapChain.hpp"

namespace RoseGold::DirectX12
{
	SwapChain::SwapChain(Device& aDevice, CommandQueue& aDirectCommandQueue, Core::Platform::Window& aWindow)
		: myDevice(&aDevice)
		, myWindow(&aWindow)
	{
		CreateRenderTextureForWindow(aDirectCommandQueue);
		UpdateColorSpace();

		Size windowSize = aWindow.GetSize();
		GetBackBuffers(windowSize);

		aWindow.SizeChanged.Connect(this, [&](Core::Platform::Window& aWindow) {
			OnDrawSurfaceResize(aWindow.GetSize());
			});
	}

	void SwapChain::Invalidate()
	{
		if (myWindow != nullptr)
		{
			myWindow->SizeChanged.Disconnect(this);
			myWindow = nullptr;
		}

		myDevice = nullptr;
		myBackBuffers.clear();
		mySwapChain.Reset();
		myDesiredResolution.reset();
	}

	void SwapChain::UpdateResolution()
	{
		DoBufferResizing();
	}

	void SwapChain::Present()
	{
		if (myBackBuffers.empty())
			return;

		mySwapChain->Present(1, 0);
	}

	unsigned int SwapChain::GetCurrentBufferIndex() const
	{
		if (myBackBuffers.empty())
			return 0;

		return mySwapChain->GetCurrentBackBufferIndex();
	}

	unsigned int SwapChain::GetBufferCount() const
	{
		return static_cast<unsigned int>(myBackBuffers.size());
	}

	void SwapChain::SetName(const wchar_t* aName)
	{
		for (std::size_t i = 0; i < myBackBuffers.size(); ++i)
		{
			myBackBuffers[i]->SetName(
				(std::wstring(aName) + L" #" + std::to_wstring(i)).c_str()
			);
		}
	}

	const DescriptorHeapHandle* SwapChain::GetColorView() const
	{
		if (myBackBuffers.empty())
			return nullptr;

		return myBackBuffers.at(GetCurrentBufferIndex())->GetColorView();
	}

	const DescriptorHeapHandle* SwapChain::GetDepthStencilView() const
	{
		if (myBackBuffers.empty())
			return nullptr;

		return myBackBuffers.at(GetCurrentBufferIndex())->GetDepthStencilView();
	}

	ID3D12Resource* SwapChain::GetColorResource() const
	{
		if (myBackBuffers.empty())
			return nullptr;

		return myBackBuffers.at(GetCurrentBufferIndex())->GetColorResource();
	}

	ID3D12Resource* SwapChain::GetDepthResource() const
	{
		if (myBackBuffers.empty())
			return nullptr;

		return myBackBuffers.at(GetCurrentBufferIndex())->GetDepthResource();
	}

	GPUResource& SwapChain::GetGPUResource()
	{
		return myBackBuffers.at(GetCurrentBufferIndex())->GetGPUResource();
	}

	GPUResource& SwapChain::GetDepthGPUResource()
	{
		return myBackBuffers.at(GetCurrentBufferIndex())->GetDepthGPUResource();
	}

	const Core::Graphics::RenderTextureDescriptor& SwapChain::GetDescriptor() const
	{
		return myBackBuffers.at(GetCurrentBufferIndex())->GetDescriptor();
	}

	void* SwapChain::GetNativeDepthBufferPtr() const
	{
		if (myBackBuffers.empty())
			return nullptr;

		return myBackBuffers.at(GetCurrentBufferIndex())->GetNativeDepthBufferPtr();
	}

	void SwapChain::SetFilterMode(Core::Graphics::FilterMode aFilterMode)
	{
		for (auto backBuffer : myBackBuffers)
			backBuffer->SetFilterMode(aFilterMode);
	}

	Core::Graphics::TextureDimension SwapChain::GetDimensions() const
	{
		return myBackBuffers.at(GetCurrentBufferIndex())->GetDimensions();
	}

	unsigned int SwapChain::GetDepth() const
	{
		return myBackBuffers.at(GetCurrentBufferIndex())->GetDepth();
	}

	Core::Graphics::FilterMode SwapChain::GetFilterMode() const
	{
		return myBackBuffers.at(GetCurrentBufferIndex())->GetFilterMode();
	}

	unsigned int SwapChain::GetHeight() const
	{
		return myBackBuffers.at(GetCurrentBufferIndex())->GetHeight();
	}

	bool SwapChain::IsReadable() const
	{
		return myBackBuffers.at(GetCurrentBufferIndex())->IsReadable();
	}

	unsigned int SwapChain::GetMipmapCount() const
	{
		return myBackBuffers.at(GetCurrentBufferIndex())->GetMipmapCount();
	}

	unsigned int SwapChain::GetWidth() const
	{
		return myBackBuffers.at(GetCurrentBufferIndex())->GetWidth();
	}

	Core::Graphics::TextureWrapMode SwapChain::GetWrapModeU() const
	{
		return myBackBuffers.at(GetCurrentBufferIndex())->GetWrapModeU();
	}

	Core::Graphics::TextureWrapMode SwapChain::GetWrapModeV() const
	{
		return myBackBuffers.at(GetCurrentBufferIndex())->GetWrapModeV();
	}

	Core::Graphics::TextureWrapMode SwapChain::GetWrapModeW() const
	{
		return myBackBuffers.at(GetCurrentBufferIndex())->GetWrapModeW();
	}

	void SwapChain::SetWrapMode(Core::Graphics::TextureWrapMode aWrapMode)
	{
		for (auto backBuffer : myBackBuffers)
			backBuffer->SetWrapMode(aWrapMode);
	}

	void SwapChain::SetWrapModeU(Core::Graphics::TextureWrapMode aWrapMode) const
	{
		for (auto backBuffer : myBackBuffers)
			backBuffer->SetWrapModeU(aWrapMode);
	}

	void SwapChain::SetWrapModeV(Core::Graphics::TextureWrapMode aWrapMode) const
	{
		for (auto backBuffer : myBackBuffers)
			backBuffer->SetWrapModeV(aWrapMode);
	}

	void SwapChain::SetWrapModeW(Core::Graphics::TextureWrapMode aWrapMode) const
	{
		for (auto backBuffer : myBackBuffers)
			backBuffer->SetWrapModeW(aWrapMode);
	}

	void* SwapChain::GetNativeTexturePtr() const
	{
		return myBackBuffers.at(GetCurrentBufferIndex())->GetNativeTexturePtr();
	}

	void SwapChain::CreateRenderTextureForWindow(CommandQueue& aDirectCommandQueue)
	{
		DXGI_SWAP_CHAIN_DESC1 swapChainDescriptor = {};
		const Size windowSize = myWindow->GetSize();
		swapChainDescriptor.Width = windowSize.X;
		swapChainDescriptor.Height = windowSize.Y;
		swapChainDescriptor.Format = ToDXGIFormat(ToGraphicsFormat(GetRenderTextureFormat()));
		swapChainDescriptor.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		swapChainDescriptor.BufferCount = 2;
		swapChainDescriptor.SampleDesc.Count = 1;
		swapChainDescriptor.SampleDesc.Quality = 0;
		swapChainDescriptor.Scaling = DXGI_SCALING_STRETCH;
		swapChainDescriptor.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
		swapChainDescriptor.AlphaMode = DXGI_ALPHA_MODE_IGNORE;
		swapChainDescriptor.Flags = myDevice->GetParameters().AllowTearing ? DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING : 0u;

		DXGI_SWAP_CHAIN_FULLSCREEN_DESC fsSwapChainDesc = {};
		fsSwapChainDesc.Windowed = TRUE;

		// Create a swap chain for the window.
		ComPtr<IDXGISwapChain1> swapChain;

		std::any surfaceHandle = myWindow->GetNativeHandle();
		HWND windowHandle = std::any_cast<HWND>(surfaceHandle);

		AssertAction(myDevice->GetFactory()->CreateSwapChainForHwnd(
			aDirectCommandQueue.GetCommandQueue().Get(),
			windowHandle,
			&swapChainDescriptor,
			&fsSwapChainDesc,
			nullptr,
			swapChain.ReleaseAndGetAddressOf()
		), "Create swapchain for window.");

		swapChain.As(&mySwapChain);

		// Todo: Ensure Alt+Enter works as intended.
		myDevice->GetFactory()->MakeWindowAssociation(windowHandle, DXGI_MWA_NO_ALT_ENTER);
	}

	void SwapChain::DoBufferResizing()
	{
		if (!myDesiredResolution.has_value())
			return;

		const Size newResolution = myDesiredResolution.value();
		myDesiredResolution.reset();

		if (newResolution.LengthSquared() == 0)
			return;

		for (std::shared_ptr<SwapChainBackBuffer>& backBuffer : myBackBuffers)
			backBuffer->Invalidate();

		UpdateColorSpace();

		myBackBuffers.clear();

		Debug::Assert(mySwapChain, "There is a swap-chain to resize.");
		HRESULT hr = mySwapChain->ResizeBuffers(
			static_cast<UINT>(myBackBuffers.size()),
			static_cast<UINT>(newResolution.X),
			static_cast<UINT>(newResolution.Y),
			ToDXGIFormat(ToGraphicsFormat(GetRenderTextureFormat())),
			myDevice->GetParameters().AllowTearing ? DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING : 0u
		);

		if (hr == DXGI_ERROR_DEVICE_REMOVED || hr == DXGI_ERROR_DEVICE_RESET)
		{
			Debug::Log("Device Lost on ResizeBuffers: Reason code 0x%08X\n",
				static_cast<unsigned int>((hr == DXGI_ERROR_DEVICE_REMOVED) ? myDevice->GetDevice()->GetDeviceRemovedReason() : hr)
			);

			// If the device was removed for any reason, a new device and swap chain will need to be created.
			__debugbreak();
			//HandleDeviceLost();

			// Everything is set up now. Do not continue execution of this method. HandleDeviceLost will reenter this method
			// and correctly set up the new device.
			return;
		}
		else
		{
			AssertAction(hr, "Unknown error when resizing swapchain.");
		}

		GetBackBuffers(newResolution);
	}

	void SwapChain::UpdateColorSpace()
	{
		DXGI_COLOR_SPACE_TYPE colorSpace = DXGI_COLOR_SPACE_RGB_FULL_G22_NONE_P709;

		bool isDisplayHDR10 = false;

		if (mySwapChain)
		{
			std::any surfaceHandle = myWindow->GetNativeHandle();
			const HWND windowHandle = std::any_cast<HWND>(surfaceHandle);
			RECT windowBounds;
			if (!GetWindowRect(windowHandle, &windowBounds))
			{
				throw std::system_error(std::error_code(static_cast<int>(GetLastError()), std::system_category()), "GetWindowRect");
				return;
			}

			const Math::RectangleT<LONG> windowRect = Math::RectangleT<LONG>::FromExtents(
				{ windowBounds.left, windowBounds.top },
				{ windowBounds.right, windowBounds.bottom }
			);

			ComPtr<IDXGIOutput> bestOutput;
			long bestIntersectArea = -1;

			ComPtr<IDXGIAdapter> adapter;
			for (UINT adapterIndex = 0;
				SUCCEEDED(myDevice->GetFactory()->EnumAdapters(adapterIndex, adapter.ReleaseAndGetAddressOf()));
				++adapterIndex)
			{
				ComPtr<IDXGIOutput> output;
				for (UINT outputIndex = 0;
					SUCCEEDED(adapter->EnumOutputs(outputIndex, output.ReleaseAndGetAddressOf()));
					++outputIndex)
				{
					// Get the rectangle bounds of current output.
					DXGI_OUTPUT_DESC desc;
					AssertAction(output->GetDesc(&desc), "Get adapter output description.");
					const Math::RectangleT<LONG> desktopCoordinates = Math::RectangleT<LONG>::FromExtents(
						{ desc.DesktopCoordinates.left, desc.DesktopCoordinates.top },
						{ desc.DesktopCoordinates.right, desc.DesktopCoordinates.bottom });

					// Compute the intersection
					auto intersection = desktopCoordinates.Intersection(windowRect);

					if (intersection.has_value() && intersection.value().Content() > bestIntersectArea)
					{
						bestOutput = output;
						bestIntersectArea = intersection.value().Content();
					}
				}
			}

			if (bestOutput)
			{
				ComPtr<IDXGIOutput6> output6;
				if (SUCCEEDED(bestOutput.As(&output6)))
				{
					DXGI_OUTPUT_DESC1 desc;
					AssertAction(output6->GetDesc1(&desc), "Get adapter output description.");

					if (desc.ColorSpace == DXGI_COLOR_SPACE_RGB_FULL_G2084_NONE_P2020)
					{
						// Display output is HDR10.
						isDisplayHDR10 = true;
					}
				}
			}
		}

		if (myDevice->GetParameters().EnableHDR && isDisplayHDR10)
		{
			switch (ToDXGIFormat(ToGraphicsFormat(GetRenderTextureFormat())))
			{
			case DXGI_FORMAT_R10G10B10A2_UNORM:
				// The application creates the HDR10 signal.
				colorSpace = DXGI_COLOR_SPACE_RGB_FULL_G2084_NONE_P2020;
				break;

			case DXGI_FORMAT_R16G16B16A16_FLOAT:
				// The system creates the HDR10 signal; application uses linear values.
				colorSpace = DXGI_COLOR_SPACE_RGB_FULL_G10_NONE_P709;
				break;

			default:
				break;
			}
		}

		UINT colorSpaceSupport = 0;
		if (mySwapChain
			&& SUCCEEDED(mySwapChain->CheckColorSpaceSupport(colorSpace, &colorSpaceSupport))
			&& (colorSpaceSupport & DXGI_SWAP_CHAIN_COLOR_SPACE_SUPPORT_FLAG_PRESENT))
		{
			AssertAction(mySwapChain->SetColorSpace1(colorSpace), "Set swapchain color space.");
		}
	}

	void SwapChain::GetBackBuffers(const Size& aSize)
	{
		myBackBuffers.clear();

		ComPtr<ID3D12Resource> backBufferResource;

		for (unsigned int i = 0; i < 2; ++i)
		{
			AssertAction(mySwapChain->GetBuffer(i, IID_PPV_ARGS(backBufferResource.ReleaseAndGetAddressOf())), "Get swapchain back buffer.");
			//backBufferResource->SetName(BasicString<wchar_t>::Format(L"Backbuffer Texture #%i", i).ToCharArray());

			Core::Graphics::RenderTextureDescriptor rtDesc;
			rtDesc.ColorFormat = GetRenderTextureFormat();
			rtDesc.ColorGraphicsFormat = ToGraphicsFormat(rtDesc.ColorFormat);
			rtDesc.DepthStencilFormat = Core::Graphics::GraphicsFormat::D32_SFloat;
			rtDesc.Dimension = Core::Graphics::TextureDimension::Tex2D;
			rtDesc.IsSRGB = false;
			rtDesc.Size_Width = aSize.X;
			rtDesc.Size_Height = aSize.Y;
			rtDesc.Size_Depth = 1;

			myBackBuffers.emplace_back(new SwapChainBackBuffer(*myDevice, rtDesc, backBufferResource, nullptr));
		}
	}

	void SwapChain::OnDrawSurfaceResize(const Size& aSize)
	{
		if (aSize.X <= 0 || aSize.Y <= 0)
			return;

		myDesiredResolution = aSize;
	}

	Core::Graphics::RenderTextureFormat SwapChain::GetRenderTextureFormat() const
	{
		if (myDevice->GetParameters().EnableHDR)
			return Core::Graphics::RenderTextureFormat::DefaultHDR;
		else
			return Core::Graphics::RenderTextureFormat::Default;
	}

	SwapChain::SwapChainBackBuffer::SwapChainBackBuffer(Device& aDevice, const Core::Graphics::RenderTextureDescriptor& aDescriptor, ComPtr<ID3D12Resource> aColorBuffer, ComPtr<ID3D12Resource> aDepthBuffer)
		: RenderTexture(aDevice, aDescriptor, aColorBuffer, aDepthBuffer)
	{
		myUsageState = D3D12_RESOURCE_STATE_PRESENT;
	}

	void SwapChain::SwapChainBackBuffer::Invalidate()
	{
		myResource.Reset();
		myDepthResource = GPUResource();
		myRSVHandle.reset();
		myDSVHandle.reset();
	}
}
