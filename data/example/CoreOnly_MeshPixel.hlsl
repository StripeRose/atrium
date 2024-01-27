
Texture2D<float4> tex : register(t0, space1);
SamplerState clampPoint : register(s0, space4);

struct PixelInput
{
    float4 ScreenPosition : SV_Position;
    float2 UV : TEXCOORD;
    float4 Color : COLOR;
};

float4 main(PixelInput anInput) : SV_TARGET
{
    const float3 textureColor = tex.Sample(clampPoint, anInput.UV).rgb;
    return float4(textureColor * anInput.Color.rgb, 1.0f);
}