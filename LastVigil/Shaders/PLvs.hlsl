cbuffer CB_VERTEX
{
    matrix model;
    matrix modelViewProj;
};

struct VOut
{
    float3 position : POSITION;
    float3 n : NORMAL;
    float2 texcoord : TEXCOORD;
    float4 pos : SV_Position;
};

VOut main(float3 position : POSITION, float3 n : NORMAL, float2 TexCoord : TEXCOORD) {
    VOut output;

    output.position = (float3) mul(float4(position, 1.0f), model);
    output.n = mul(n, (float3x3) model);
    output.texcoord = TexCoord;
    output.pos = mul(float4(output.position, 1.0f), modelViewProj);

    return output;
}
