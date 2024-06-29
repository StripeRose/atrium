#define Space_PerObject space0
#define Space_PerMaterial space1
#define Space_PerPass space2
#define Space_PerFrame space3
#define Space_Constant space4

cbuffer Camera : register(b0, Space_PerObject)
{
    row_major float4x4 ModelMatrix : packoffset(c0);
    row_major float4x4 ViewMatrix : packoffset(c4);
    row_major float4x4 ProjectionMatrix : packoffset(c8);
};

float4 TransformPosition(in float3 aPosition)
{
    const float4 worldPosition = mul(float4(aPosition, 1.0f), ModelMatrix);
    const float4 cameraSpacePosition = mul(worldPosition, ViewMatrix);
    const float4 projectionPosition = mul(cameraSpacePosition, ProjectionMatrix);
    
    return projectionPosition;
}
