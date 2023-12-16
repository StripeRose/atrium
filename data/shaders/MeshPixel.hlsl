struct PixelInput
{
    float3 Color : COLOR;
};

float4 main(PixelInput anInput) : SV_TARGET
{
    return float4(anInput.Color, 1.0f);

}