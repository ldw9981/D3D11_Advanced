//--------------------------------------------------------------------------------------
// Constant Buffer Variables
//--------------------------------------------------------------------------------------
Texture2D txDiffuse : register(t0);
Texture2D txNormal : register(t1);
Texture2D txSpecular : register(t2);
Texture2D txEmissive : register(t3);
Texture2D txOpacity : register(t4);

TextureCube txCube : register(t7);


SamplerState samLinear : register(s0);
SamplerState samTexture3D : register(s1);

cbuffer Transform : register(b0)
{
    matrix World;
    matrix View;
    matrix Projection;
}

cbuffer DirectionLight : register(b1)
{
    float3 LightDirection;
    float DirectionLightPad0;
    float4 LightAmbient;
    float4 LightDiffuse;
    float4 LightSpecular;
    float3 EyePosition;
    float DirectionLightPad1;
}

cbuffer Material : register(b2)
{
    float4 MaterialAmbient;
    float4 MaterialDiffuse;
    float4 MaterialSpecular;
    float4 MaterialEmissive;
    float MaterialSpecularPower;
    bool UseDiffuseMap;
    bool UseNormalMap;
    bool UseSpecularMap;    
    bool UseEmissiveMap;
    bool UseOpacityMap;
    float2 MaterialPad0;
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


struct PS_INPUT_SKYBOX
{  
    float4 PositionProj : SV_POSITION;
    float3 TexCoords : TEXCOORD0;
};