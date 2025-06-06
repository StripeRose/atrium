// Filter "Resources"

#include "DX12_CommandQueue.hpp"
#include "DX12_Device.hpp"
#include "DX12_Diagnostics.hpp"
#include "DX12_SwapChain.hpp"

// There should be one more back-buffer than the amount of frames in flight,
// so one can be presenting while the others are currently being processed.
#define DX12_BACKBUFFER_COUNT (DX12_FRAMES_IN_FLIGHT + 1)

namespace Atrium::DirectX12
{
	SwapChain::SwapChain(Device& aDevice, CommandQueue& aDirectCommandQueue, Atrium::Core::Window& aWindow)
		: myDevice(&aDevice)
		, myWindow(&aWindow)
	{
		CreateRenderTextureForWindow(aDirectCommandQueue);
		UpdateColorSpace();

		SizeT<int> windowSize = aWindow.GetSize();
		GetBackBuffers(windowSize);

		aWindow.OnSizeChanged.Connect(this, [&]() { OnDrawSurfaceResize(aWindow.GetSize()); });
	}

	SwapChain::~SwapChain()
	{
		Invalidate();
	}

	void SwapChain::Invalidate()
	{
		if (myWindow != nullptr)
		{
			myWindow->OnSizeChanged.Disconnect(this);
			myWindow = nullptr;
		}

		myDevice = nullptr;
		myBackBuffers.clear();
		mySwapChain.Reset();
		myDesiredResolution.reset();
	}

	bool SwapChain::NeedsResize() const
	{
		return myDesiredResolution.has_value();
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

	DescriptorHeapHandle SwapChain::GetColorView() const
	{
		if (myBackBuffers.empty())
			return DescriptorHeapHandle();

		return myBackBuffers.at(GetCurrentBufferIndex())->GetColorView();
	}

	DescriptorHeapHandle SwapChain::GetDepthStencilView() const
	{
		if (myBackBuffers.empty())
			return DescriptorHeapHandle();

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

	std::shared_ptr<GPUResource> SwapChain::GetGPUResource()
	{
		return myBackBuffers.at(GetCurrentBufferIndex())->GetGPUResource();
	}

	std::shared_ptr<GPUResource> SwapChain::GetDepthGPUResource()
	{
		return myBackBuffers.at(GetCurrentBufferIndex())->GetDepthGPUResource();
	}

	const Core::RenderTextureDescriptor& SwapChain::GetDescriptor() const
	{
		return myBackBuffers.at(GetCurrentBufferIndex())->GetDescriptor();
	}

	void* SwapChain::GetNativeDepthBufferPtr() const
	{
		if (myBackBuffers.empty())
			return nullptr;

		return myBackBuffers.at(GetCurrentBufferIndex())->GetNativeDepthBufferPtr();
	}

	Core::TextureDimension SwapChain::GetDimensions() const
	{
		return myBackBuffers.at(GetCurrentBufferIndex())->GetDimensions();
	}

	unsigned int SwapChain::GetDepth() const
	{
		return myBackBuffers.at(GetCurrentBufferIndex())->GetDepth();
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

	void* SwapChain::GetNativeTexturePtr() const
	{
		return myBackBuffers.at(GetCurrentBufferIndex())->GetNativeTexturePtr();
	}

	void SwapChain::CreateRenderTextureForWindow(CommandQueue& aDirectCommandQueue)
	{
		DXGI_SWAP_CHAIN_DESC1 swapChainDescriptor = {};
		const SizeT<int> windowSize = myWindow->GetSize();
		swapChainDescriptor.Width = windowSize.Width;
		swapChainDescriptor.Height = windowSize.Height;
		swapChainDescriptor.Format = ToDXGIFormat(ToGraphicsFormat(GetRenderTextureFormat()));
		swapChainDescriptor.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		swapChainDescriptor.BufferCount = DX12_BACKBUFFER_COUNT;
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

		Debug::Assert(myDevice->GetFactory()->CreateSwapChainForHwnd(
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

	void SwapChain::TriggerResize()
	{
		if (!myDesiredResolution.has_value())
			return;

		const SizeT<int> newResolution = myDesiredResolution.value();
		myDesiredResolution.reset();

		if ((newResolution.Width * newResolution.Height) == 0)
			return;

		for (std::shared_ptr<SwapChainBackBuffer>& backBuffer : myBackBuffers)
			backBuffer->Invalidate();

		UpdateColorSpace();

		myBackBuffers.clear();

		Debug::Assert(mySwapChain, "There is a swap-chain to resize.");
		HRESULT hr = mySwapChain->ResizeBuffers(
			static_cast<UINT>(myBackBuffers.size()),
			static_cast<UINT>(newResolution.Width),
			static_cast<UINT>(newResolution.Height),
			ToDXGIFormat(ToGraphicsFormat(GetRenderTextureFormat())),
			myDevice->GetParameters().AllowTearing ? DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING : 0u
		);

		if (hr == DXGI_ERROR_DEVICE_REMOVED || hr == DXGI_ERROR_DEVICE_RESET)
		{
			HRESULT removalReason = myDevice->GetDevice()->GetDeviceRemovedReason();
			removalReason;

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
			Debug::Assert(hr, "Unknown error when resizing swapchain.");
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

			const RectangleT<int> windowRect = RectangleT<int>(
				PointT<int>(windowBounds.left, windowBounds.top),
				PointT<int>(windowBounds.right, windowBounds.bottom)
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
					Debug::Assert(output->GetDesc(&desc), "Get adapter output description.");
					const RectangleT<int> desktopCoordinates = RectangleT<int>(
						PointT<int>(desc.DesktopCoordinates.left, desc.DesktopCoordinates.top),
						PointT<int>(desc.DesktopCoordinates.right, desc.DesktopCoordinates.bottom));

					// Compute the intersection
					auto intersection = desktopCoordinates.Intersection(windowRect);

					if (intersection.has_value() && intersection.value().Area() > bestIntersectArea)
					{
						bestOutput = output;
						bestIntersectArea = intersection.value().Area();
					}
				}
			}

			if (bestOutput)
			{
				ComPtr<IDXGIOutput6> output6;
				if (SUCCEEDED(bestOutput.As(&output6)))
				{
					DXGI_OUTPUT_DESC1 desc;
					Debug::Assert(output6->GetDesc1(&desc), "Get adapter output description.");

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
			Debug::Assert(mySwapChain->SetColorSpace1(colorSpace), "Set swapchain color space.");
		}
	}

	void SwapChain::GetBackBuffers(const SizeT<int>& aSize)
	{
		myBackBuffers.clear();

		ComPtr<ID3D12Resource> backBufferResource;

		for (unsigned int i = 0; i < DX12_BACKBUFFER_COUNT; ++i)
		{
			Debug::Assert(mySwapChain->GetBuffer(i, IID_PPV_ARGS(backBufferResource.ReleaseAndGetAddressOf())), "Get swapchain back buffer.");
			//backBufferResource->SetName(BasicString<wchar_t>::Format(L"Backbuffer Texture #%i", i).ToCharArray());

			Core::RenderTextureDescriptor rtDesc;
			rtDesc.ColorFormat = GetRenderTextureFormat();
			rtDesc.ColorGraphicsFormat = ToGraphicsFormat(rtDesc.ColorFormat);
			rtDesc.DepthStencilFormat = Core::GraphicsFormat::D32_SFloat;
			rtDesc.Dimension = Core::TextureDimension::Tex2D;
			rtDesc.IsSRGB = false;
			rtDesc.Size_Width = aSize.Width;
			rtDesc.Size_Height = aSize.Height;
			rtDesc.Size_Depth = 1;

			myBackBuffers.emplace_back(new SwapChainBackBuffer(*myDevice, rtDesc, backBufferResource, nullptr));
		}
	}

	void SwapChain::OnDrawSurfaceResize(const SizeT<int>& aSize)
	{
		if (aSize.Width <= 0 || aSize.Height <= 0)
			return;

		myDesiredResolution = aSize;
	}

	Core::RenderTextureFormat SwapChain::GetRenderTextureFormat() const
	{
		if (myDevice->GetParameters().EnableHDR)
			return Core::RenderTextureFormat::DefaultHDR;
		else
			return Core::RenderTextureFormat::Default;
	}

	SwapChain::SwapChainBackBuffer::SwapChainBackBuffer(Device& aDevice, const Core::RenderTextureDescriptor& aDescriptor, const ComPtr<ID3D12Resource>& aColorBuffer, const ComPtr<ID3D12Resource>& aDepthBuffer)
		: RenderTexture(
			aDevice,
			aDescriptor,
			aColorBuffer ? std::make_shared<GPUResource>(aColorBuffer, D3D12_RESOURCE_STATE_PRESENT) : nullptr,
			aDepthBuffer ? std::make_shared<GPUResource>(aDepthBuffer) : nullptr
		)
	{

	}

	void SwapChain::SwapChainBackBuffer::Invalidate()
	{
		myResource.reset();
		myDepthResource.reset();
		myRSVHandle.Invalidate();
		myDSVHandle.Invalidate();
	}
}
