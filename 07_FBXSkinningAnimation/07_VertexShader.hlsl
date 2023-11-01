#include "Shared.hlsli"

//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------
PS_INPUT main(VS_INPUT input)
{
    PS_INPUT output = (PS_INPUT) 0;
    float4 pos = input.PositionModel;

    float4x4 matWorldBlended;
    matWorldBlended = mul(input.BlendWeights.x, MatrixPaletteArray[input.BlendIndices.x]);
    matWorldBlended += mul(input.BlendWeights.y, MatrixPaletteArray[input.BlendIndices.y]);
    matWorldBlended += mul(input.BlendWeights.z, MatrixPaletteArray[input.BlendIndices.z]);
    matWorldBlended += mul(input.BlendWeights.w, MatrixPaletteArray[input.BlendIndices.w]);
    
    pos = mul(pos, matWorldBlended);
    output.PositionWorld = pos.xyz;
    
    pos = mul(pos, View);
    pos = mul(pos, Projection);  
    output.PositionProj = pos;    
    output.TexCoord = input.TexCoord;    
    output.NormalWorld = normalize(mul(input.NormalModel, (float3x3) matWorldBlended)); // scale 있을수 있으므로 normalize필요
    output.TangentWorld = normalize(mul(input.TangentModel, (float3x3) matWorldBlended)); // scale 있을수 있으므로 normalize필요
    return output;
}