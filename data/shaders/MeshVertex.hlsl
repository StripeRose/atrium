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
    float4 Position : SV_Position;
    float3 Normal : NORMAL;
    float2 UV : TEXCOORD;
    float3 Color : COLOR;
    
    float3 Binormal : BINORMAL;
    float3 Tangent : TANGENT;
};

VertexOutput main(VertexInput anInput)
{
    float4x4 modelViewProjection = mul(ModelMatrix, mul(ViewMatrix, ProjectionMatrix));
    
    VertexOutput output;
    output.Position = mul(float4(anInput.Position, 1.0f), modelViewProjection);
    output.Normal = anInput.Normal;
    output.UV = anInput.UV;
    output.Color = anInput.Color;
    
    output.Binormal = anInput.Binormal;
    output.Tangent = anInput.Tangent;
    
    return output;
}