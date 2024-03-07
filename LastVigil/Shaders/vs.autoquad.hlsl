cbuffer CB_PROJ
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

static const float3 position[4] = { -0.5f, 0.0f,-0.5f,-0.5f, 0.0f, 0.5f, 0.5f, 0.0f,-0.5f, 0.5f, 0.0f, 0.5f };
    
VOut main(float2 TexCoord : TEXCOORD, uint Index : BLENDINDICES) {
    VOut output;
    
    output.position = (float3) mul(float4(position[Index], 1.0f), model);
    output.n = mul(float3(0.0f, 1.0f, 0.0f), (float3x3) model);
    output.texcoord = TexCoord;
    output.pos = mul(float4(output.position, 1.0f), modelViewProj);

    return output;
}
