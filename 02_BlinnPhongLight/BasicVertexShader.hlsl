#include "Shared.hlsli"

//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------
PS_INPUT main(VS_INPUT input)
{
    PS_INPUT output = (PS_INPUT) 0;
    float4 pos = input.PosModel;
    pos = mul(pos, World);
    output.PosWorld = pos.xyz;
    
    pos = mul(pos, View);
    pos = mul(pos, Projection);  
    output.PosProj = pos;    
    output.TexCoord = input.TexCoord;    
    output.NorWorld = normalize(mul(input.NorModel, (float3x3) World));    // scale 있을수 있으므로 normalize필요
    return output;
}