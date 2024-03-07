struct VOut
{
    float4 position : SV_POSITION;
    float3 n : NORMAL;
    float2 texcoord : TEXCOORD;
};

cbuffer CONSTANTBUFFER {
   matrix transform;
};

VOut VShader(float4 position : POSITION, float3 n : NORMAL, float2 TexCoord : TEXCOORD)
{
    VOut output;

    output.position = mul(position, transform);
    output.n = n;
    output.texcoord = TexCoord;

    return output;
}
