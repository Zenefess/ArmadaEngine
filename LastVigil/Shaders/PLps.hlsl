Texture2D Texture : register(t0);

SamplerState Sampler : register(s0);

struct LIGHT {
    float4 lightPos; // .w == range
    float4 lightCol; // .a == flags
};

cbuffer cbLight {
    LIGHT l[16];    // 256 bytes
}

static const float3 ambient = { 0.15f, 0.15f, 0.15f };

float4 main(float3 position : POSITION, float3 n : NORMAL, float2 TexCoord : TEXCOORD) : SV_Target
{
    const float4 Texel = Texture.Sample(Sampler, TexCoord);

    if (Texel.a < 0.707106f) discard;

    float3 result = { 0.0f, 0.0f, 0.0f };

    for (uint xx = 0 ; xx < 16 && l[xx].lightCol.a != 0xFFFFFFFF; xx++)
    {
        const float3 lCol    = l[xx].lightCol.rgb;
        const float  range   = l[xx].lightPos.w;
        const float3 vToL    = l[xx].lightPos.xyz - position;
        const float  distToL = length(vToL);
        
        if (distToL < range * 2.0f)
        {
            const float  att = min(1.0f, (distToL / range + distToL / (range * range)) * 0.5f);
            const float3 lum = Texel.rgb * saturate(dot(vToL / distToL, n)) * lCol;
            if (att >= 0.0f) result += lum * (1.0f - att);  // Metallic
            else result -= att * lCol + lum;                // Non-metallic
        }
    }
    return float4(ambient * Texel.rgb + result, Texel.a);
}
