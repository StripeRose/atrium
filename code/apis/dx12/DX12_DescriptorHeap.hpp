#pragma once

#include "DX12_ComPtr.hpp"

#include <d3d12.h>

#include <memory>
#include <vector>

namespace RoseGold::DirectX12
{
    class DescriptorHeap;
    class DescriptorHeapHandle
    {
        friend DescriptorHeap;
    public:
        D3D12_CPU_DESCRIPTOR_HANDLE GetCPUHandle() const;
        D3D12_CPU_DESCRIPTOR_HANDLE GetCPUHandle(unsigned int anIndex) const;
        D3D12_GPU_DESCRIPTOR_HANDLE GetGPUHandle() const;
        D3D12_GPU_DESCRIPTOR_HANDLE GetGPUHandle(unsigned int anIndex) const;
        std::uint32_t GetHeapIndex() const { return IsValid() ? myData->myHeapIndex : 0; }

        bool IsValid() const { return myData && myData->myCPUHandle.ptr != 0; }
        bool IsReferencedByShader() const { return IsValid() && myData->myGPUHandle.ptr != 0; }

        void Invalidate();


    private:
        struct HandleData
        {
            ~HandleData();
            DescriptorHeap* myHeap = nullptr;
            D3D12_CPU_DESCRIPTOR_HANDLE myCPUHandle = { 0 };
            D3D12_GPU_DESCRIPTOR_HANDLE myGPUHandle = { 0 };
            std::uint32_t myHeapIndex = 0;
        };

        std::shared_ptr<HandleData> myData;
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
        DescriptorHeapHandle CreateHeapHandle(D3D12_CPU_DESCRIPTOR_HANDLE aCPUHandle, D3D12_GPU_DESCRIPTOR_HANDLE aGPUHandle, std::uint32_t aHeapIndex);
        virtual void FreeHeapHandle(std::uint32_t) { }

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

        DescriptorHeapHandle GetNewHeapHandle();
        void FreeHeapHandle(std::uint32_t anIndex) override;

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
        DescriptorHeapHandle GetHeapHandleBlock(std::uint32_t aCount);

    private:
        std::uint32_t myCurrentDescriptorIndex;
    };
}