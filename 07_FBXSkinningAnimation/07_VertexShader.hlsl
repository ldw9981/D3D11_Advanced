#include "Shared.hlsli"

//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------
PS_INPUT main(VS_INPUT input)
{
    PS_INPUT output = (PS_INPUT) 0;
    float4 pos = input.PositionModel;

    float4x4 matWorldBlended;
    matWorldBlended = mul(input.BlendWeights.x, MatrixPalleteArray[input.BlendIndices.x]);
    matWorldBlended += mul(input.BlendWeights.y, MatrixPalleteArray[input.BlendIndices.y]);
    matWorldBlended += mul(input.BlendWeights.z, MatrixPalleteArray[input.BlendIndices.z]);
    matWorldBlended += mul(input.BlendWeights.w, MatrixPalleteArray[input.BlendIndices.w]);
    
    pos = mul(pos, matWorldBlended);
    output.PositionWorld = pos.xyz;
    
    pos = mul(pos, View);
    pos = mul(pos, Projection);  
    output.PositionProj = pos;    
    output.TexCoord = input.TexCoord;    
    output.NormalWorld = normalize(mul(input.NormalModel, (float3x3) matWorldBlended)); // scale ������ �����Ƿ� normalize�ʿ�
    output.TangentWorld = normalize(mul(input.TangentModel, (float3x3) matWorldBlended)); // scale ������ �����Ƿ� normalize�ʿ�
    return output;
}