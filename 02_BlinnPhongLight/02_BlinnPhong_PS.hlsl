#include "Shared.hlsli"

//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
float4 main(PS_INPUT input) : SV_Target
{
    float3 vNormal = normalize(input.NorWorld);
    float fNDotL = max(dot(vNormal, -LightDirection), 0);
    float3 vView = normalize(EyePosition - input.PosWorld.xyz);    
    float4 Ambient = LightAmbient * MaterialAmbient;
    float4 Diffuse = LightDiffuse * MaterialDiffuse * txDiffuse.Sample(samLinear, input.TexCoord) * fNDotL;
    
    float fSDot = 0.0f;
    if (UseBlinnPhong)
    {
        float3 HalfVector = normalize(-LightDirection + vView);
        fSDot = max(dot(HalfVector, vNormal), 0);
    }
    else
    {
        float3 vReflection = reflect(LightDirection, vNormal);
        fSDot = max(dot(vReflection, vView), 0);
    }  
    float4 Specular = pow(fSDot, MaterialSpecularPower) * LightSpecular * MaterialSpecular;
    
    float4 FinalColor = Diffuse + Specular + Ambient;
    return FinalColor;
}
