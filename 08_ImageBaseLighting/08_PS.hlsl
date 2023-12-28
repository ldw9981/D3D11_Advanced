#include "Shared.hlsli"

//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
float4 main(PS_INPUT input) : SV_Target
{
    float3 vNormal = normalize(input.NormalWorld);
    float3 vTangent = normalize(input.TangentWorld);
    float3 vBiTanget = cross(vNormal, vTangent);
    float Opacity = 1.0f;
    
    if (UseNormalMap)
    {
        float3 vNormalTangentSpace = txNormal.Sample(samLinear, input.TexCoord).rgb * 2.0f - 1.0f;
        float3x3 WorldTransform = float3x3(vTangent, vBiTanget, vNormal);
        vNormal = mul(vNormalTangentSpace, WorldTransform); 
        vNormal = normalize(vNormal);
    }
    
    float4 fTxDiffuse = txDiffuse.Sample(samLinear, input.TexCoord);
    float fNDotL = max(dot(vNormal, -LightDirection), 0);
    float3 vView = normalize(EyePosition - input.PositionWorld.xyz);    
    float4 Ambient = LightAmbient * MaterialAmbient * fTxDiffuse;   
    float4 Diffuse = LightDiffuse * MaterialDiffuse * fNDotL;
    if (UseDiffuseMap)
    {
        Diffuse *= fTxDiffuse; 
    }

 
    // ºí¸°Æþ
    float3 HalfVector = normalize(-LightDirection + vView);
    float fSDot = max(dot(HalfVector, vNormal), 0);
    float4 Specular = pow(fSDot, MaterialSpecularPower) * LightSpecular * MaterialSpecular;
    if (UseSpecularMap)
    {
        Specular *= txSpecular.Sample(samLinear, input.TexCoord);
    }
    
    float4 Emissive = 0;
    if (UseEmissiveMap)
    {
        Emissive = txEmissive.Sample(samLinear, input.TexCoord) * MaterialEmissive;
    }
    
    if (UseOpacityMap)
    {
        Opacity = txOpacity.Sample(samLinear, input.TexCoord).a;
    }    
    
    float4 FinalColor = Diffuse + Ambient + Specular + Emissive;
    return float4(FinalColor.rgb,Opacity);
}
