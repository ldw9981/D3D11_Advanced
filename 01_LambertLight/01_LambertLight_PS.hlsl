#include "Shared.hlsli"

//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
float4 main(PS_INPUT input) : SV_Target
{
    float fNDotL = dot(input.Nor, -LightDirection);
    
    float4 FinalColor = saturate( LightColor * txDiffuse.Sample(samLinear, input.Tex) * fNDotL);
    
    return FinalColor;
}
