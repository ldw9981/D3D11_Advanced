//--------------------------------------------------------------------------------------
// Constant Buffer Variables
//--------------------------------------------------------------------------------------
Texture2D txDiffuse : register(t0);
Texture2D txNormal : register(t1);

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
    float4 LightAmbient;
    float4 LightDiffuse;
    float4 LightSpecular;
    float3 EyePosition;
    bool UseNormalMap;
}

cbuffer Material : register(b2)
{
    float4 MaterialAmbient;
    float4 MaterialDiffuse;
    float4 MaterialSpecular;
    float MaterialSpecularPower;
    float3 Materialpad0;
}


//--------------------------------------------------------------------------------------
struct VS_INPUT
{
    float4 PositionModel : POSITION;    
    float2 TexCoord : TEXCOORD0;
    float3 NormalModel : NORMAL;
    float3 TangentModel : TANGENT;
};

struct PS_INPUT
{
    float4 PositionProj : SV_POSITION;   
    float3 PositionWorld : POSITION;
    float2 TexCoord : TEXCOORD0;
    float3 NormalWorld : NORMAL;
    float3 TangentWorld : TANGENT;
};
