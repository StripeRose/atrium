// Filter "Resources"

#include "DX12_Mesh.hpp"

#include "DX12_Diagnostics.hpp"
#include "DX12_Manager.hpp"

#include <filesystem>
#include <fstream>

namespace RoseGold::DirectX12
{
	ComPtr<ID3D12RootSignature> Mesh::ourMeshRootSignature;
	std::unique_ptr<char> Mesh::ourVertexShaderData;
	D3D12_SHADER_BYTECODE Mesh::ourVertexShaderBytecode;
	std::unique_ptr<char> Mesh::ourPixelShaderData;
	D3D12_SHADER_BYTECODE Mesh::ourPixelShaderBytecode;

	void Mesh::Prepare(ID3D12Device* aDevice)
	{
		CreateRootSignature(aDevice);
		CreateShaders();
	}

	ID3D12RootSignature* Mesh::GetRootSignature()
	{
		return ourMeshRootSignature.Get();
	}

	void Mesh::Cleanup()
	{
		ourMeshRootSignature.Reset();
		ourVertexShaderData.reset();
		ourVertexShaderBytecode.BytecodeLength = 0;
		ourPixelShaderData.reset();
		ourPixelShaderBytecode.BytecodeLength = 0;
	}

	Mesh::Mesh(Manager& aManager)
		: Core::Graphics::Mesh(aManager)
	{ }

	void Mesh::CreateRootSignature(ID3D12Device * aDevice)
	{
		D3D12_FEATURE_DATA_ROOT_SIGNATURE featureData = { };
		featureData.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_1;

		if (FAILED(aDevice->CheckFeatureSupport(D3D12_FEATURE_ROOT_SIGNATURE, &featureData, sizeof(featureData))))
		{
			featureData.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_0;
		}

		D3D12_DESCRIPTOR_RANGE1 ranges[1];
		ranges[0].BaseShaderRegister = 0;
		ranges[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_CBV;
		ranges[0].NumDescriptors = 1;
		ranges[0].RegisterSpace = 0;
		ranges[0].OffsetInDescriptorsFromTableStart = 0;
		ranges[0].Flags = D3D12_DESCRIPTOR_RANGE_FLAG_NONE;

		D3D12_ROOT_PARAMETER1 rootParameters[1];
		rootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
		rootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;

		rootParameters[0].DescriptorTable.NumDescriptorRanges = 1;
		rootParameters[0].DescriptorTable.pDescriptorRanges = ranges;

		D3D12_VERSIONED_ROOT_SIGNATURE_DESC rootSignatureDesc;
		rootSignatureDesc.Version = D3D_ROOT_SIGNATURE_VERSION_1_1;
		rootSignatureDesc.Desc_1_1.Flags =
			D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
		rootSignatureDesc.Desc_1_1.NumParameters = 1;
		rootSignatureDesc.Desc_1_1.pParameters = rootParameters;
		rootSignatureDesc.Desc_1_1.NumStaticSamplers = 0;
		rootSignatureDesc.Desc_1_1.pStaticSamplers = nullptr;

		ComPtr<ID3DBlob> signature;
		ComPtr<ID3DBlob> error;

		HRESULT result = S_OK;
		result = D3D12SerializeVersionedRootSignature(&rootSignatureDesc, &signature, &error);
		if (SUCCEEDED(result))
			result = aDevice->CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(), IID_PPV_ARGS(ourMeshRootSignature.ReleaseAndGetAddressOf()));

		if (FAILED(result))
		{
			LogError(result, error.Get());
			return;
		}

		ourMeshRootSignature->SetName(L"DirectX12::Mesh root signature");
	}

	void Mesh::CreateShaders()
	{
		/*auto readShaderFile = [](const std::filesystem::path& aFilePath, std::unique_ptr<char>& someData, D3D12_SHADER_BYTECODE& aBytecodeEntry)
		{
			std::ifstream fileStream(aFilePath, std::ios::ate | std::ios::binary);
			if (!fileStream.is_open())
				return;

			const std::size_t fileSize = fileStream.tellg();
			fileStream.seekg(0);

			someData = std::unique_ptr<char>(new char[fileSize]);
			fileStream.read(someData.get(), fileSize);
			fileStream.close();

			aBytecodeEntry.pShaderBytecode = someData.get();
			aBytecodeEntry.BytecodeLength = fileSize;
		};

		readShaderFile(L"lib/MeshVertex.cso", ourVertexShaderData, ourVertexShaderBytecode);
		readShaderFile(L"lib/MeshPixel.cso", ourPixelShaderData, ourPixelShaderBytecode);*/
	}
}