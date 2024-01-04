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
    output.NormalWorld = normalize(mul(input.NormalModel, (float3x3) World));    // scale 있을수 있으므로 normalize필요
    output.TangentWorld = normalize(mul(input.TangentModel, (float3x3) World)); // scale 있을수 있으므로 normalize필요
    return output;
}


PS_INPUT_SKYBOX main_skybox(VS_INPUT input)
{
    PS_INPUT_SKYBOX output = (PS_INPUT_SKYBOX) 0;        
    float4 pos = mul(input.PositionModel, World);
    output.TexCoords = pos.xyz;  
    float3 pos3 = mul(pos.xyz,(float3x3)View); // 역행렬에서 위치는 적용안함
    pos = mul(float4(pos3, 1.0f), Projection);
    output.PositionProj = pos;    
    return output;
}