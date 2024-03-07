Texture2D Texture : register(t0);
Texture2D Texture2 : register(t1);

SamplerState Sampler : register(s0);

float4 PShader(float4 position : SV_POSITION, float3 n : NORMAL, float2 TexCoord : TEXCOORD) : SV_TARGET
{
   const float4 Texel = Texture2.Sample(Sampler, TexCoord);
    
   if (Texel.a > 0.292893f) discard;
    
   return Texel;
}
