#include "Shared.hlsli"

//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
float4 main(PS_INPUT input) : SV_Target
{
    float3 vLight = -LightDirection;
    float3 fvNormal = normalize(input.NorWorld);
    float fNDotL = max(dot(fvNormal, vLight), 0);
    float3 vEyeDir = normalize(EyePosition - input.PosWorld.xyz);
    float3 vReflection = -reflect(vLight, input.NorWorld);
    
    
 
    float3 Ambient = float4(0.0f,0.0f,0.0f,0.0f);
    float3 Diffuse = txDiffuse.Sample(samLinear, input.TexCoord) * fNDotL;
    float3 Specular = pow(max(dot(vEyeDir, vReflection), 0), SpecularPower);
    
    float4 FinalColor = float4(Diffuse+Specular, 1.0f);
    return FinalColor;
}
