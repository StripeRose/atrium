#pragma once

#include "DX12_ComPtr.hpp"

#include <d3d12.h>

#include <memory>
#include <vector>

namespace RoseGold::DirectX12
{
    // Todo: Make compatible with ComPtrs.
    class DescriptorHeapHandle
    {
        friend class DescriptorHeap;

        DescriptorHeapHandle(DescriptorHeap* aHeapOwner)
            : myHeap(aHeapOwner)
        {
            myCPUHandle.ptr = 0;
            myGPUHandle.ptr = 0;
            myHeapIndex = 0;
        }

    public:
        DescriptorHeapHandle() : DescriptorHeapHandle(nullptr) { }
        ~DescriptorHeapHandle();

        D3D12_CPU_DESCRIPTOR_HANDLE GetCPUHandle() const { return myCPUHandle; }
        D3D12_GPU_DESCRIPTOR_HANDLE GetGPUHandle() const { return myGPUHandle; }
        std::uint32_t GetHeapIndex() const { return myHeapIndex; }

        void SetCPUHandle(D3D12_CPU_DESCRIPTOR_HANDLE aCPUHandle) { myCPUHandle = aCPUHandle; }
        void SetGPUHandle(D3D12_GPU_DESCRIPTOR_HANDLE aGPUHandle) { myGPUHandle = aGPUHandle; }
        void SetHeapIndex(std::uint32_t aHeapIndex) { myHeapIndex = aHeapIndex; }

        bool IsValid() const { return myCPUHandle.ptr != 0; }
        bool IsReferencedByShader() const { return myGPUHandle.ptr != 0; }

        DescriptorHeapHandle& operator=(const DescriptorHeapHandle&) = delete;

    private:
        DescriptorHeap* myHeap;
        D3D12_CPU_DESCRIPTOR_HANDLE myCPUHandle;
        D3D12_GPU_DESCRIPTOR_HANDLE myGPUHandle;
        std::uint32_t myHeapIndex;
    };

    class DescriptorHeap
    {
        friend DescriptorHeapHandle;
    public:
        DescriptorHeap(ComPtr<ID3D12Device> aDevice, D3D12_DESCRIPTOR_HEAP_TYPE aHeapType, std::uint32_t aNumDescriptors, bool anIsReferencedByShader);
        virtual ~DescriptorHeap() = default;

        ComPtr<ID3D12DescriptorHeap> GetHeap() { return myDescriptorHeap; }
        D3D12_DESCRIPTOR_HEAP_TYPE GetHeapType() const { return myHeapType; }
        D3D12_CPU_DESCRIPTOR_HANDLE GetHeapCPUStart() const { return myDescriptorHeapCPUStart; }
        D3D12_GPU_DESCRIPTOR_HANDLE GetHeapGPUStart() const { return myDescriptorHeapGPUStart; }
        std::uint32_t GetMaxDescriptors() const { return myMaxDescriptors; }
        std::uint32_t GetDescriptorSize() const { return myDescriptorSize; }

    protected:
        std::shared_ptr<DescriptorHeapHandle> CreateHeapHandle();
        virtual void FreeHeapHandle(const DescriptorHeapHandle&) { }

    protected:
        ComPtr<ID3D12DescriptorHeap> myDescriptorHeap;
        D3D12_DESCRIPTOR_HEAP_TYPE myHeapType;
        D3D12_CPU_DESCRIPTOR_HANDLE myDescriptorHeapCPUStart;
        D3D12_GPU_DESCRIPTOR_HANDLE myDescriptorHeapGPUStart;
        std::uint32_t myMaxDescriptors;
        std::uint32_t myDescriptorSize;
        bool myIsReferencedByShader;
    };

    class StagingDescriptorHeap : public DescriptorHeap
    {
    public:
        StagingDescriptorHeap(ComPtr<ID3D12Device> aDevice, D3D12_DESCRIPTOR_HEAP_TYPE aHeapType, std::uint32_t aNumDescriptors);
        ~StagingDescriptorHeap() override;

        std::shared_ptr<DescriptorHeapHandle> GetNewHeapHandle();
        void FreeHeapHandle(const DescriptorHeapHandle& handle) override;

    private:
        std::vector<std::uint32_t> myFreeDescriptors;
        std::uint32_t myCurrentDescriptorIndex;
        std::uint32_t myActiveHandleCount;
    };

    class RenderPassDescriptorHeap : public DescriptorHeap
    {
    public:
        RenderPassDescriptorHeap(ComPtr<ID3D12Device> aDevice, D3D12_DESCRIPTOR_HEAP_TYPE aHeapType, std::uint32_t aNumDescriptors);

        void Reset();
        std::shared_ptr<DescriptorHeapHandle> GetHeapHandleBlock(std::uint32_t aCount);

    private:
        std::uint32_t myCurrentDescriptorIndex;
    };
}