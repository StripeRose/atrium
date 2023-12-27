cbuffer Camera : register(b0)
{
    row_major float4x4 ProjectionMatrix : packoffset(c0);
    row_major float4x4 ModelMatrix : packoffset(c4);
    row_major float4x4 ViewMatrix : packoffset(c8);
};

struct VertexInput
{
    float3 Position : POSITION;
    float3 Normal : NORMAL;
    float2 UV : TEXCOORD;
    float3 Color : COLOR;
    
    float3 Binormal : BINORMAL;
    float3 Tangent : TANGENT;
};

struct VertexOutput
{
    float4 ScreenPosition : SV_Position;
    float3 WorldPosition : POSITION0;
    float2 ScreenCoordinate : POSITION1;
    
    float3 Normal : NORMAL;
    float3 Binormal : BINORMAL;
    float3 Tangent : TANGENT;
    
    float2 UV : TEXCOORD;
    float3 Color : COLOR;
};

VertexOutput main(VertexInput anInput)
{
    const float4 worldPosition = mul(float4(anInput.Position, 1.0f), ModelMatrix);
    const float4 cameraSpacePosition = mul(worldPosition, ViewMatrix);
    const float4 projectionPosition = mul(cameraSpacePosition, ProjectionMatrix);
    
    const float3 worldNormal = normalize(float3(mul(float4(anInput.Normal, 0), ModelMatrix).xyz));
    const float3 worldBiNormal = normalize(float3(mul(float4(anInput.Binormal, 0), ModelMatrix).xyz));
    const float3 worldTangentNormal = normalize(float3(mul(float4(anInput.Tangent, 0), ModelMatrix).xyz));
    
    VertexOutput output;
    
    output.WorldPosition = worldPosition.xyz;
    output.ScreenPosition = projectionPosition / projectionPosition.w;
    
    output.ScreenCoordinate = output.ScreenPosition.xy;
    output.ScreenCoordinate.y *= -1;
    output.ScreenCoordinate /= 2;
    output.ScreenCoordinate += 0.5;
    
    output.Normal = worldNormal;
    output.Binormal = worldBiNormal;
    output.Tangent = worldTangentNormal;
    
    output.UV = anInput.UV;
    output.Color = anInput.Color;
    
    return output;
}