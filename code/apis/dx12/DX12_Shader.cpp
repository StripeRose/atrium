// Filter "Resources"

#include "DX12_Diagnostics.hpp"
#include "DX12_Shader.hpp"

#include <d3dcompiler.h>

#pragma comment(lib, "d3dcompiler.lib")

namespace RoseGold::DirectX12
{
    std::shared_ptr<Shader> Shader::CreateFromSource(const std::filesystem::path& aFilePath, const char* anEntryPoint, const char* aProfile)
    {
        if (aFilePath.empty() || !anEntryPoint || !aProfile)
            return nullptr;

        UINT flags = D3DCOMPILE_ENABLE_STRICTNESS;
#if _DEBUG
        flags |= D3DCOMPILE_DEBUG;
#endif

        /*const D3D_SHADER_MACRO defines[] =
        {
            "EXAMPLE_DEFINE", "1",
            NULL, NULL
        };*/

        ComPtr<ID3DBlob> shaderBlob;
        ComPtr<ID3DBlob> errorBlob;
        HRESULT hr = D3DCompileFromFile(
            aFilePath.c_str(), nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE,
            anEntryPoint,
            aProfile, flags, 0, shaderBlob.ReleaseAndGetAddressOf(), errorBlob.ReleaseAndGetAddressOf()
        );

        if (!LogError(hr, errorBlob.Get()))
            return nullptr;

        std::shared_ptr<Shader> shader(new Shader());

        shader->myDataBlob = shaderBlob;
        shader->myByteCode.pShaderBytecode = shader->myDataBlob->GetBufferPointer();
        shader->myByteCode.BytecodeLength = shader->myDataBlob->GetBufferSize();

        return shader;
    }
}
