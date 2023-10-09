#include "Shared.hlsli"

//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------
PS_INPUT main(VS_INPUT input)
{
    PS_INPUT output = (PS_INPUT) 0;
    float4 pos = input.PositionModel;
    pos = mul(pos, World);
    output.PositionWorld = pos.xyz;
    
    pos = mul(pos, View);
    pos = mul(pos, Projection);  
    output.PositionProj = pos;    
    output.TexCoord = input.TexCoord;    
    output.NormalWorld = normalize(mul(input.NormalModel, (float3x3) World));    // scale ������ �����Ƿ� normalize�ʿ�
    output.TangentWorld = normalize(mul(input.TangentModel, (float3x3) World)); // scale ������ �����Ƿ� normalize�ʿ�
    return output;
}