/************************************************************
 * File: ps.16lights.fractal.hlsl       Created: 2022/11/03 *
 *                                Last modified: 2022/11/03 *
 *                                                          *
 * Desc:                                                    *
 *                                                          *
 *  Copyright (c) David William Bull. All rights reserved.  *
 ************************************************************/

//Texture2D Texture[4] : register(t0);

//SamplerState Sampler : register(s0);

struct LIGHT {
	 float4 lightPos; // .w == range
	 float4 lightCol; // .a == flags
};

cbuffer cbLight {
	 LIGHT l[16] : register(b0); // 512 bytes
}

struct GOut {
	float4 pos : SV_Position;
	float3 position : POSITION;
	float3 n : NORMAL;
	float2 tex : TEXCOORD;
	uint   pID : SV_PrimitiveID;
};

static const float3 ambient = { 0.0f, 0.0f, 0.0f };

float3 hash3( float2 p ){
		float3 q = float3( dot(p,float2(127.1,311.7)), 
				dot(p,float2(269.5,183.3)), 
				dot(p,float2(419.2,371.9)) );
	return frac(sin(q)*43758.5453);
}

float iqnoise( in float2 x, float u, float v ){
		float2 p = floor(x);
		float2 f = frac(x);
		
	float k = 1.0+63.0*pow(1.0-v,4.0);
	
	float va = 0.0;
	float wt = 0.0;
		for( int j=-2; j<=2; j++ )
		for( int i=-2; i<=2; i++ )
		{
			float2 g = float2( float(i),float(j) );
		float3 o = hash3( p + g )*float3(u,u,1.0);
		float2 r = g - f + o.xy;
		float d = dot(r,r);
		float ww = pow(abs(1.0-smoothstep(0.0,1.414,sqrt(d))), k );
		va += o.z*ww;
		wt += ww;
		}
	
		return va/wt;
}

inline float4 Mandelbrot(float2 os, float2 scale, float dist, uint iter) {
	const float cy = -(os.x - 0.5f) * 2.0f * scale.x;
	const float cx = -(os.y - 0.5f) * 2.0f * scale.y;
			float zx = 0;
			float zy = 0;
			uint  i  = 0;

	dist *= scale.x * scale.y;
	while((zx * zx + zy * zy < dist) && (i < iter)) {
		float tmp = tan(zx * zx) - zx + cx;
		zy = 2.0f * sin(zx * zy) + cy;
		zx = tmp;
		i++;
	}

	const float v = (float(i) / float(iter));
	const float3 col = min(1.0f, 1.0f - log(float3(v * 2.0f, v * 3.0, v)));
	return float4(col, v);
}

float4 main(in GOut g) : SV_Target {
	const float2 fOS    = { 0.0f, 0.0f };
	const float2 fScale = { 1.0f, 1.0f };

//			float4 Texel = Mandelbrot(g.tex + fOS, fScale, 1.0f, 256);
			float4 Texel = iqnoise(g.position.xy * 0.5, g.tex.x, g.tex.y);

//    clip(Texel.a - 0.70710678118654752440084436210485f);
//    clip(Texel.a - 0.25f);
	 clip(Texel.a - 0.01f);

//	return Texel;
//}
	 
	 float3 result = { 0.0f, 0.0f, 0.0f };

	 float3 lCol = l[0].lightCol.rgb;
	 float range = l[0].lightPos.w;
	 float3 vToL = l[0].lightPos.xyz - g.position;
	 float distToL = length(vToL);
	 float att = min(1.0f, (distToL / range + distToL / (range * range)) * 0.5f);
	 float3 lum = saturate(dot(vToL / distToL, g.n)) * lCol;
	 result += lum * (1.0f - att);

	 lCol = l[1].lightCol.rgb;
	 range = l[1].lightPos.w;
	 vToL = l[1].lightPos.xyz - g.position;
	 distToL = length(vToL);
	 att = min(1.0f, (distToL / range + distToL / (range * range)) * 0.5f);
	 lum = saturate(dot(vToL / distToL, g.n)) * lCol;
	 result += lum * (1.0f - att);

	 lCol = l[2].lightCol.rgb;
	 range = l[2].lightPos.w;
	 vToL = l[2].lightPos.xyz - g.position;
	 distToL = length(vToL);
	 att = min(1.0f, (distToL / range + distToL / (range * range)) * 0.5f);
	 lum = saturate(dot(vToL / distToL, g.n)) * lCol;
	 result += lum * (1.0f - att);

	 lCol = l[3].lightCol.rgb;
	 range = l[3].lightPos.w;
	 vToL = l[3].lightPos.xyz - g.position;
	 distToL = length(vToL);
	 att = min(1.0f, (distToL / range + distToL / (range * range)) * 0.5f);
	 lum = saturate(dot(vToL / distToL, g.n)) * lCol;
	 result += lum * (1.0f - att);

	 lCol = l[4].lightCol.rgb;
	 range = l[4].lightPos.w;
	 vToL = l[4].lightPos.xyz - g.position;
	 distToL = length(vToL);
	 att = min(1.0f, (distToL / range + distToL / (range * range)) * 0.5f);
	 lum = saturate(dot(vToL / distToL, g.n)) * lCol;
	 result += lum * (1.0f - att);

	 lCol = l[5].lightCol.rgb;
	 range = l[5].lightPos.w;
	 vToL = l[5].lightPos.xyz - g.position;
	 distToL = length(vToL);
	 att = min(1.0f, (distToL / range + distToL / (range * range)) * 0.5f);
	 lum = saturate(dot(vToL / distToL, g.n)) * lCol;
	 result += lum * (1.0f - att);

	 lCol = l[6].lightCol.rgb;
	 range = l[6].lightPos.w;
	 vToL = l[6].lightPos.xyz - g.position;
	 distToL = length(vToL);
	 att = min(1.0f, (distToL / range + distToL / (range * range)) * 0.5f);
	 lum = saturate(dot(vToL / distToL, g.n)) * lCol;
	 result += lum * (1.0f - att);

	 lCol = l[7].lightCol.rgb;
	 range = l[7].lightPos.w;
	 vToL = l[7].lightPos.xyz - g.position;
	 distToL = length(vToL);
	 att = min(1.0f, (distToL / range + distToL / (range * range)) * 0.5f);
	 lum = saturate(dot(vToL / distToL, g.n)) * lCol;
	 result += lum * (1.0f - att);

	 lCol = l[8].lightCol.rgb;
	 range = l[8].lightPos.w;
	 vToL = l[8].lightPos.xyz - g.position;
	 distToL = length(vToL);
	 att = min(1.0f, (distToL / range + distToL / (range * range)) * 0.5f);
	 lum = saturate(dot(vToL / distToL, g.n)) * lCol;
	 result += lum * (1.0f - att);

	 lCol = l[9].lightCol.rgb;
	 range = l[9].lightPos.w;
	 vToL = l[9].lightPos.xyz - g.position;
	 distToL = length(vToL);
	 att = min(1.0f, (distToL / range + distToL / (range * range)) * 0.5f);
	 lum = saturate(dot(vToL / distToL, g.n)) * lCol;
	 result += lum * (1.0f - att);

	 lCol = l[10].lightCol.rgb;
	 range = l[10].lightPos.w;
	 vToL = l[10].lightPos.xyz - g.position;
	 distToL = length(vToL);
	 att = min(1.0f, (distToL / range + distToL / (range * range)) * 0.5f);
	 lum = saturate(dot(vToL / distToL, g.n)) * lCol;
	 result += lum * (1.0f - att);

	 lCol = l[11].lightCol.rgb;
	 range = l[11].lightPos.w;
	 vToL = l[11].lightPos.xyz - g.position;
	 distToL = length(vToL);
	 att = min(1.0f, (distToL / range + distToL / (range * range)) * 0.5f);
	 lum = saturate(dot(vToL / distToL, g.n)) * lCol;
	 result += lum * (1.0f - att);

	 lCol = l[12].lightCol.rgb;
	 range = l[12].lightPos.w;
	 vToL = l[12].lightPos.xyz - g.position;
	 distToL = length(vToL);
	 att = min(1.0f, (distToL / range + distToL / (range * range)) * 0.5f);
	 lum = saturate(dot(vToL / distToL, g.n)) * lCol;
	 result += lum * (1.0f - att);

	 lCol = l[13].lightCol.rgb;
	 range = l[13].lightPos.w;
	 vToL = l[13].lightPos.xyz - g.position;
	 distToL = length(vToL);
	 att = min(1.0f, (distToL / range + distToL / (range * range)) * 0.5f);
	 lum = saturate(dot(vToL / distToL, g.n)) * lCol;
	 result += lum * (1.0f - att);

	 lCol = l[14].lightCol.rgb;
	 range = l[14].lightPos.w;
	 vToL = l[14].lightPos.xyz - g.position;
	 distToL = length(vToL);
	 att = min(1.0f, (distToL / range + distToL / (range * range)) * 0.5f);
	 lum = saturate(dot(vToL / distToL, g.n)) * lCol;
	 result += lum * (1.0f - att);

	 lCol = l[15].lightCol.rgb;
	 range = l[15].lightPos.w;
	 vToL = l[15].lightPos.xyz - g.position;
	 distToL = length(vToL);
	 att = min(1.0f, (distToL / range + distToL / (range * range)) * 0.5f);
	 lum = saturate(dot(vToL / distToL, g.n)) * lCol;
	 result += lum * (1.0f - att);

	 return float4((ambient + result) * Texel.rgb, Texel.a);
}