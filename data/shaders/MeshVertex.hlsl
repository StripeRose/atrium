cbuffer Camera : register(b0)
{
    row_major float4x4 ProjectionMatrix : packoffset(c0);
    row_major float4x4 ModelMatrix : packoffset(c4);
    row_major float4x4 ViewMatrix : packoffset(c8);
};

struct GenericMeshInput
{
    float3 Position : POSITION;
    float3 Normal : NORMAL;
    float3 Binormal : BINORMAL;
    float3 Tangent : TANGENT;
    float2 UV : TEXCOORD;
};

struct ColoredMeshInput
{
    float3 Position : POSITION;
    float3 Normal : NORMAL;
    float3 Binormal : BINORMAL;
    float3 Tangent : TANGENT;
    float2 UV : TEXCOORD;
    float4 Color : COLOR;
};

struct SimpleMeshInput
{
    float3 Position : POSITION;
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
    float4 Color : COLOR;
};

VertexOutput DefaultOutput()
{
    VertexOutput output;
    
    output.ScreenPosition = float4(0, 0, 0, 1);
    output.WorldPosition = float3(0, 0, 0);
    output.ScreenCoordinate = float2(0.5f, 0.5f);
    
    output.Normal = float3(1, 0, 0);
    output.Tangent = float3(0, 1, 0);
    output.Binormal = float3(0, 0, 1);
    
    output.UV = float2(0, 0);
    output.Color = float4(1, 1, 1, 1);
    
    return output;
}

void Populate_Position(inout VertexOutput aVertex, in float3 aPosition)
{
    const float4 worldPosition = mul(float4(aPosition, 1.0f), ModelMatrix);
    const float4 cameraSpacePosition = mul(worldPosition, ViewMatrix);
    const float4 projectionPosition = mul(cameraSpacePosition, ProjectionMatrix);
    
    aVertex.WorldPosition = worldPosition.xyz;
    aVertex.ScreenPosition = projectionPosition / projectionPosition.w;
    aVertex.ScreenCoordinate = aVertex.ScreenPosition.xy;
    aVertex.ScreenCoordinate.y *= -1;
    aVertex.ScreenCoordinate /= 2;
    aVertex.ScreenCoordinate += 0.5;
}

void Populate_Normal(inout VertexOutput aVertex, in float3 aNormal)
{
    aVertex.Normal = normalize(float3(mul(float4(aNormal, 0), ModelMatrix).xyz));
}

void Populate_Binormal(inout VertexOutput aVertex, in float3 aBinormal)
{
    aVertex.Binormal = normalize(float3(mul(float4(aBinormal, 0), ModelMatrix).xyz));
}

void Populate_Tangent(inout VertexOutput aVertex, in float3 aTangent)
{
    aVertex.Tangent = normalize(float3(mul(float4(aTangent, 0), ModelMatrix).xyz));
}

VertexOutput GenericMesh(GenericMeshInput anInput)
{
    VertexOutput output = DefaultOutput();
    
    Populate_Position(output, anInput.Position);
    Populate_Normal(output, anInput.Normal);
    Populate_Binormal(output, anInput.Binormal);
    Populate_Tangent(output, anInput.Tangent);
    
    output.UV = anInput.UV;
    
    return output;
}

VertexOutput ColoredMesh(ColoredMeshInput anInput)
{
    VertexOutput output = DefaultOutput();
    
    Populate_Position(output, anInput.Position);
    Populate_Normal(output, anInput.Normal);
    Populate_Binormal(output, anInput.Binormal);
    Populate_Tangent(output, anInput.Tangent);
    
    output.UV = anInput.UV;
    output.Color = anInput.Color;
    
    return output;
}

VertexOutput SimpleMesh(SimpleMeshInput anInput)
{
    VertexOutput output = DefaultOutput();
    
    Populate_Position(output, anInput.Position);
    
    return output;
}
