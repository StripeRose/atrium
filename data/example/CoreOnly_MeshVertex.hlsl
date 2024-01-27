cbuffer Camera : register(b0)
{
    row_major float4x4 ProjectionMatrix : packoffset(c0);
    row_major float4x4 ModelMatrix : packoffset(c4);
    row_major float4x4 ViewMatrix : packoffset(c8);
};

struct ColoredVertex
{
    float3 Position : POSITION;
    float4 Color : COLOR;
};

struct TexturedVertex
{
    float3 Position : POSITION;
    float2 UV : TEXCOORD;
};

struct VertexOutput
{
    float4 ScreenPosition : SV_Position;
    
    float2 UV : TEXCOORD;
    float4 Color : COLOR;
};

float4 TransformPosition(in float3 aPosition)
{
    const float4 worldPosition = mul(float4(aPosition, 1.0f), ModelMatrix);
    const float4 cameraSpacePosition = mul(worldPosition, ViewMatrix);
    const float4 projectionPosition = mul(cameraSpacePosition, ProjectionMatrix);
    
    return projectionPosition;
}

VertexOutput ColoredMesh(ColoredVertex anInput)
{
    VertexOutput output;
    
    output.ScreenPosition = TransformPosition(anInput.Position);
    output.UV = float2(0.5f, 0.5f);
    output.Color = anInput.Color;
    
    return output;
}

VertexOutput TexturedMesh(TexturedVertex anInput)
{
    VertexOutput output;
    
    output.ScreenPosition = TransformPosition(anInput.Position);
    output.UV = anInput.UV;
    output.Color = float4(1, 1, 1, 1);
    
    return output;
}
