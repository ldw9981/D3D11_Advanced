#include "Shared.hlsli"

//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
float4 main(PS_INPUT input) : SV_Target
{
    float3 vNormal = normalize(input.NorWorld);
    float fNDotL = max(dot(vNormal, -LightDirection), 0);
    float3 vEyeDir = normalize(EyePosition - input.PosWorld.xyz);
    float3 vReflection = reflect(LightDirection, input.NorWorld);
    float4 Ambient = LightAmbient * MaterialAmbient;
    float4 Diffuse = LightDiffuse * MaterialDiffuse * txDiffuse.Sample(samLinear, input.TexCoord) * fNDotL;
    
    float fRDotE = max(dot(vReflection, vEyeDir), 0);
    float4 Specular = pow(fRDotE, MaterialSpecularPower) * LightSpecular * MaterialSpecular;
    
    float4 FinalColor = Diffuse + Specular + Ambient;
    return FinalColor;
}
