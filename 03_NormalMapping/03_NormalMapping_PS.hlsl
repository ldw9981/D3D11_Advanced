#include "Shared.hlsli"

//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
float4 main(PS_INPUT input) : SV_Target
{
    float3 vNormal = normalize(input.NormalWorld);
    float3 vTangent = normalize(input.TangentWorld);
    float3 vBiTanget = cross(vNormal, vTangent);
    
    if (UseNormalMap)
    {
        float3 vNormalTangentSpace = txNormal.Sample(samLinear, input.TexCoord).rgb * 2.0f - 1.0f;
        float3x3 TBN = float3x3(vTangent, vBiTanget, vNormal);
        vNormal = mul(vNormalTangentSpace, TBN);
        vNormal = normalize(vNormal);
    }
    
    float fNDotL = max(dot(vNormal, -LightDirection), 0);
    float3 vView = normalize(EyePosition - input.PositionWorld.xyz);    
    float4 Ambient = LightAmbient * MaterialAmbient;
    float4 fTxDiffuse = txDiffuse.Sample(samLinear, input.TexCoord);
    float4 Diffuse = LightDiffuse * MaterialDiffuse * fTxDiffuse * fNDotL;
   
    // ºí¸°Æþ
    float3 HalfVector = normalize(-LightDirection + vView);
    float fSDot = max(dot(HalfVector, vNormal), 0);
 

    float4 Specular = pow(fSDot, MaterialSpecularPower) * LightSpecular * MaterialSpecular;
    
    float4 FinalColor = Diffuse + Specular + Ambient;
    return FinalColor;
}
