#include "Common.hlsl"

// Pipeline resources
//Texture2D<float4> tex : register(t0, Space_PerMaterial);
SamplerState clampPoint : register(s0, Space_Constant);

struct FretboardVertex
{
    float3 Position : POSITION;
};

struct PixelData
{
    float4 ScreenPosition : SV_Position;
    float2 UV : TEXCOORD;
    float4 Color : COLOR;
};

PixelData vertexShader(FretboardVertex anInput)
{
    PixelData pixelData;

    pixelData.ScreenPosition = TransformPosition(anInput.Position);
    pixelData.UV = float2(0, 0);
    pixelData.Color = float4(0.5, 0.5, 0.5, 1);

    return pixelData;
}

float4 pixelShader(PixelData anInput) : SV_TARGET
{
    //const float3 textureColor = tex.Sample(clampPoint, anInput.UV).rgb;
    //return float4(textureColor * anInput.Color.rgb, 1.0f);
    
    return float4(anInput.Color.rgb, 1.0f);

}
