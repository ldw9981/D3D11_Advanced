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
    float DL_pad0;
    float3 LightColor;
    float DL_pad1;
    float3 EyePosition;
    bool useBlinnPhong;
}

cbuffer Material : register(b2)
{
    float3 Ambient;
    float M_pad0;
    float3 Diffuse;
    float M_pad1;
    float3 Specular;
    float SpecularPower;
}


//--------------------------------------------------------------------------------------
struct VS_INPUT
{
    float4 PosModel : POSITION;    
    float2 TexCoord : TEXCOORD0;
    float3 NorModel : NORMAL;
};

struct PS_INPUT
{
    float4 PosProj : SV_POSITION;   
    float3 PosWorld : POSITION;
    float2 TexCoord : TEXCOORD0;
    float3 NorWorld : NORMAL;
};
