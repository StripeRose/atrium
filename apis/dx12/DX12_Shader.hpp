// Filter "Resources"

#pragma once

#include "DX12_ComPtr.hpp"

#include "Graphics_Pipeline.hpp"

#include <d3d12.h>

#include <filesystem>
#include <memory>

namespace Atrium::DirectX12
{
	class Shader : public Core::Shader
	{
	public:
		static std::shared_ptr<Shader> CreateFromSource(const std::filesystem::path& aFilePath, const char* anEntryPoint, const char* aProfile);

	public:
		D3D12_SHADER_BYTECODE GetByteCode() const { return myByteCode; }

	private:
		ComPtr<ID3DBlob> myDataBlob;
		D3D12_SHADER_BYTECODE myByteCode;
	};
}