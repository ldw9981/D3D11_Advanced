#include "Shared.hlsli"

//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
float4 main(PS_INPUT input) : SV_Target
{
    float3 vNormal = normalize(input.NorWorld);
    float fNDotL = max(dot(vNormal, -LightDirection), 0);
    float3 vEyeDir = normalize(EyePosition - input.PosWorld.xyz);
    float3 vReflection = -reflect(-LightDirection, input.NorWorld);
    
    
 
    float3 Ambient = float4(0.0f,0.0f,0.0f,0.0f);
    float3 Diffuse = txDiffuse.Sample(samLinear, input.TexCoord) * fNDotL;
    
    float fRDotE = max(dot(vReflection, vEyeDir), 0);
    float3 Specular = pow(fRDotE, SpecularPower);
    
    float4 FinalColor = float4(Specular, 1.0f);
    return FinalColor;
}
