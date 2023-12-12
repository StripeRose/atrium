// Filter "Resources"

#include "DX12_Device.hpp"
#include "DX12_Diagnostics.hpp"
#include "DX12_SwapChain.hpp"

#include "Common_Math.hpp"

namespace RoseGold::DirectX12
{
	SwapChain::SwapChain(Device& aDevice, Core::Platform::Window& aWindow)
		: myDevice(&aDevice)
		, myWindow(&aWindow)
	{
		CreateSwapChain();
		UpdateColorSpace();

		int windowWidth, windowHeight;
		aWindow.GetSize(windowWidth, windowHeight);
		GetBackBuffers(windowWidth, windowHeight);

		aWindow.OnSizeChanged.Connect(this, [&](int aWidth, int aHeight) {
			OnDrawSurfaceResize(aWidth, aHeight);
			});
	}

	SwapChain::~SwapChain()
	{
		/*CommandQueue& queue = myDevicePtr->GetCommandQueueManager().GetGraphicsQueue();
		queue.WaitForFenceCPUBlocking(queue.InsertSignal());*/
	}

	void SwapChain::UpdateResolution()
	{
		DoBufferResizing();
	}

	void SwapChain::Present()
	{
		myDevice->GetCommandQueueManager().GetGraphicsQueue().InsertSignal();
		mySwapChain->Present(1, 0);
	}

	RenderTexture* SwapChain::GetRenderTexture()
	{
		if (myBackBuffers.size() > GetCurrentBufferIndex())
			return myBackBuffers.at(GetCurrentBufferIndex()).get();
		else
			return nullptr;
	}

	unsigned int SwapChain::GetCurrentBufferIndex() const
	{
		return mySwapChain->GetCurrentBackBufferIndex();
	}

	unsigned int SwapChain::GetBufferCount() const
	{
		return 2;
	}

	void SwapChain::CreateSwapChain()
	{
		DXGI_SWAP_CHAIN_DESC1 swapChainDescriptor = {};
		int windowWidth, windowHeight;
		myWindow->GetSize(windowWidth, windowHeight);
		swapChainDescriptor.Width = windowWidth;
		swapChainDescriptor.Height = windowHeight;
		swapChainDescriptor.Format = ToDXGIFormat(ToGraphicsFormat(GetRenderTextureFormat()));
		swapChainDescriptor.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		swapChainDescriptor.BufferCount = GetBufferCount();
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

		AssertSuccess(myDevice->GetFactory()->CreateSwapChainForHwnd(
			myDevice->GetCommandQueueManager().GetGraphicsQueue().GetCommandQueue().Get(),
			windowHandle,
			&swapChainDescriptor,
			&fsSwapChainDesc,
			nullptr,
			swapChain.ReleaseAndGetAddressOf()
		));

		swapChain.As(&mySwapChain);

		// Todo: Ensure Alt+Enter works as intended.
		myDevice->GetFactory()->MakeWindowAssociation(windowHandle, DXGI_MWA_NO_ALT_ENTER);
	}

	void SwapChain::DoBufferResizing()
	{
		if (!myDesiredResolution.has_value())
			return;

		const std::pair<unsigned int, unsigned int> newResolution = myDesiredResolution.value();
		myDesiredResolution.reset();

		if ((newResolution.first * newResolution.second) == 0)
			return;

		CommandQueue& queue = myDevice->GetCommandQueueManager().GetGraphicsQueue();
		queue.WaitForFenceCPUBlocking(queue.InsertSignal());

		for (std::shared_ptr<SwapChainBackBuffer>& backBuffer : myBackBuffers)
			backBuffer->Invalidate();

		UpdateColorSpace();

		myBackBuffers.clear();

		Debug::Assert(mySwapChain, "Resizing a draw surface without a swapchain.");
		HRESULT hr = mySwapChain->ResizeBuffers(
			GetBufferCount(),
			newResolution.first,
			newResolution.second,
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
			AssertSuccess(hr);
		}

		GetBackBuffers(newResolution.first, newResolution.second);
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
					AssertSuccess(output->GetDesc(&desc));
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
					AssertSuccess(output6->GetDesc1(&desc));

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
			AssertSuccess(mySwapChain->SetColorSpace1(colorSpace));
		}
	}

	void SwapChain::GetBackBuffers(int aWidth, int aHeight)
	{
		myBackBuffers.clear();

		ComPtr<ID3D12Resource> backBufferResource;

		for (unsigned int i = 0; i < GetBufferCount(); ++i)
		{
			AssertSuccess(mySwapChain->GetBuffer(i, IID_PPV_ARGS(backBufferResource.ReleaseAndGetAddressOf())));
			//backBufferResource->SetName(BasicString<wchar_t>::Format(L"Backbuffer Texture #%i", i).ToCharArray());

			Core::Graphics::RenderTextureDescriptor rtDesc;
			rtDesc.ColorFormat = GetRenderTextureFormat();
			rtDesc.ColorGraphicsFormat = ToGraphicsFormat(rtDesc.ColorFormat);
			rtDesc.DepthStencilFormat = Core::Graphics::GraphicsFormat::D32_SFloat;
			rtDesc.Dimension = Core::Graphics::TextureDimension::Tex2D;
			rtDesc.IsSRGB = false;
			rtDesc.Size_Width = aWidth;
			rtDesc.Size_Height = aHeight;
			rtDesc.Size_Depth = 1;

			myBackBuffers.emplace_back(new SwapChainBackBuffer(*myDevice, rtDesc, backBufferResource, nullptr));
		}
	}

	void SwapChain::OnDrawSurfaceResize(int aWidth, int aHeight)
	{
		myDesiredResolution = std::make_pair(aWidth, aHeight);
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
	{ }

	void SwapChain::SwapChainBackBuffer::Invalidate()
	{
		myColorBuffer.Reset();
		myDepthBuffer.Reset();
		myRSVHandle.reset();
		myDSVHandle.reset();
	}
}
