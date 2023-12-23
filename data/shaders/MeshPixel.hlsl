struct PixelInput
{
    float4 Position : SV_Position;
    float3 Normal : NORMAL;
    float2 UV : TEXCOORD;
    float3 Color : COLOR;
    
    float3 Binormal : BINORMAL;
    float3 Tangent : TANGENT;
};

float4 main(PixelInput anInput) : SV_TARGET
{
    return float4(anInput.Color, 1.0f);

}