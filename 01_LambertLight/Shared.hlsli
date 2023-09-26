//--------------------------------------------------------------------------------------
// Constant Buffer Variables
//--------------------------------------------------------------------------------------
Texture2D txDiffuse : register(t0);
SamplerState samLinear : register(s0);

cbuffer Transform : register(b0)
{
    matrix World;
    matrix View;
    matrix Projection;
}

cbuffer DirectionLight : register(b1)
{
    float3 LightDirection;
    float pad0;
    float4 LightColor;
}


//--------------------------------------------------------------------------------------
struct VS_INPUT
{
    float4 Pos : POSITION;    
    float2 Tex : TEXCOORD0;
    float3 Nor : NORMAL0;
};

struct PS_INPUT
{
    float4 Pos : SV_POSITION;   
    float2 Tex : TEXCOORD0;
    float3 Nor : NORMAL0;
};
