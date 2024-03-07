/**********************************************************************
 * File: gs.mesh displacement functions.hlsli     Created: 2023/05/13 *
 *                                              Last mod.: 2023/05/21 *
 *                                                                    *
 * Desc: Common shader functions.                                     *
 *                                                                    *
 *       Method categories: 0~7==Quad2 wave, 8~15==Acute edge         *
 *                                                                    *
 *       Copyright (c) David William Bull. All rights reserved.       *
 **********************************************************************/

#include "common.hlsli"

inline const float3 WarpCoord(in const float3 position, in const float warpStrength) {
	const uint2 seed = asint(position.yx * 0.125f + position.xy);

	return float3((asfloat(asint(rand_ui15(seed)) - 16384) * rcp16384.xx * warpStrength.xx + position.xy), position.z);
}

inline const float3x18 WarpCoord(in const float3x18 position, in const float warpStrength) {
	const  int2x18 seed = { asint(position[0].yx * 0.125f + position[0].xy), asint(position[1].yx * 0.125f + position[1].xy),
									asint(position[2].yx * 0.125f + position[2].xy), asint(position[3].yx * 0.125f + position[3].xy),
									asint(position[4].yx * 0.125f + position[4].xy), asint(position[5].yx * 0.125f + position[5].xy),
									asint(position[6].yx * 0.125f + position[6].xy), asint(position[7].yx * 0.125f + position[7].xy),
									asint(position[8].yx * 0.125f + position[8].xy), asint(position[9].yx * 0.125f + position[9].xy),
									asint(position[10].yx * 0.125f + position[10].xy), asint(position[11].yx * 0.125f + position[11].xy),
									asint(position[12].yx * 0.125f + position[12].xy), asint(position[13].yx * 0.125f + position[13].xy),
									asint(position[14].yx * 0.125f + position[14].xy), asint(position[15].yx * 0.125f + position[15].xy),
									asint(position[16].yx * 0.125f + position[16].xy), asint(position[17].yx * 0.125f + position[17].xy) };

	cfloat3x18 result = { (float2(rand_ui15(seed[0]) - 16384).xy * rcp16384.xx * warpStrength.xx + position[0].xy), position[0].z,
								 (float2(rand_ui15(seed[1]) - 16384).xy * rcp16384.xx * warpStrength.xx + position[1].xy), position[1].z,
								 (float2(rand_ui15(seed[2]) - 16384).xy * rcp16384.xx * warpStrength.xx + position[2].xy), position[2].z,
								 (float2(rand_ui15(seed[3]) - 16384).xy * rcp16384.xx * warpStrength.xx + position[3].xy), position[3].z,
								 (float2(rand_ui15(seed[4]) - 16384).xy * rcp16384.xx * warpStrength.xx + position[4].xy), position[4].z,
								 (float2(rand_ui15(seed[5]) - 16384).xy * rcp16384.xx * warpStrength.xx + position[5].xy), position[5].z,
								 (float2(rand_ui15(seed[6]) - 16384).xy * rcp16384.xx * warpStrength.xx + position[6].xy), position[6].z,
								 (float2(rand_ui15(seed[7]) - 16384).xy * rcp16384.xx * warpStrength.xx + position[7].xy), position[7].z,
								 (float2(rand_ui15(seed[8]) - 16384).xy * rcp16384.xx * warpStrength.xx + position[8].xy), position[8].z,
								 (float2(rand_ui15(seed[9]) - 16384).xy * rcp16384.xx * warpStrength.xx + position[9].xy), position[9].z,
								 (float2(rand_ui15(seed[10]) - 16384).xy * rcp16384.xx * warpStrength.xx + position[10].xy), position[10].z,
								 (float2(rand_ui15(seed[11]) - 16384).xy * rcp16384.xx * warpStrength.xx + position[11].xy), position[11].z,
								 (float2(rand_ui15(seed[12]) - 16384).xy * rcp16384.xx * warpStrength.xx + position[12].xy), position[12].z,
								 (float2(rand_ui15(seed[13]) - 16384).xy * rcp16384.xx * warpStrength.xx + position[13].xy), position[13].z,
								 (float2(rand_ui15(seed[14]) - 16384).xy * rcp16384.xx * warpStrength.xx + position[14].xy), position[14].z,
								 (float2(rand_ui15(seed[15]) - 16384).xy * rcp16384.xx * warpStrength.xx + position[15].xy), position[15].z,
								 (float2(rand_ui15(seed[16]) - 16384).xy * rcp16384.xx * warpStrength.xx + position[16].xy), position[16].z,
								 (float2(rand_ui15(seed[17]) - 16384).xy * rcp16384.xx * warpStrength.xx + position[17].xy), position[17].z };
	
	return result;
}

// Single vertex
inline const float3 Displace4x4(in const matrix density, in const float2 position, in const uint method) {
	switch(method >> 3) {
	case 0:	// Quad2 waves
		const float2 fBias_ = (position - 0.5f);
		const float2 fBias  = (0.25f - (fBias_ * fBias_)) * 2.0f;

		const vector vInner = lerp(density[1], density[2], position.x);
		const vector vOuter = lerp((density[1] - density[0]) * 0.5f + density[1], (density[2] - density[3]) * 0.5f + density[2], position.x);
		const vector vXLerp = lerp(vInner, vOuter, fBias.x);
		const float  vMajor = lerp(vXLerp[1], vXLerp[2], position.y);
		const float  vMinor = lerp((vXLerp[1] - vXLerp[0]) * 0.5f + vXLerp[1], (vXLerp[2] - vXLerp[3]) * 0.5f + vXLerp[2], position.y);

		const float result = lerp(vMajor, vMinor, fBias.y);

		switch(method & 0x07) {
		case 0:	// 00==Standard
			return float3(position, result);
		case 1:	// 02==Smoothstep
			return float3(position, -smoothstep(0.0f, 1.0f, result));
		case 2:	// 01==Flatten bottom
			return float3(position, -max(0.0f, result));
		case 3:	// 01==Flatten top
			return float3(position, -min(1.0f, result));
		}
		break;
	case 1:	// Cube
		return float3(position, density[2][2]);
	}
}

// 16-triangle strip for 16x8 mesh
inline const float3x18 Displace4x4_16x8(in const matrix density, in const float3 origin, in const uint strip, in const uint method) {
	float3x18 vertices;
	uint      x, y;

	switch(method >> 3) {
	case 0:	// Quad2 waves
		switch(method & 0x07) {
		case 0:	// 00==Standard
			[unroll]
			for(x = 0; x < 9; x++)
				[unroll]
				for(y = 0; y < 2; y++) {
					const float2 position = float2(x, y + strip) * 0.125f;

					const float2 fBias_ = (position - 0.5f);
					const float2 fBias  = (0.25f - (fBias_ * fBias_)) * 2.0f;

					const vector vInner = lerp(density[1], density[2], position.x);
					const vector vOuter = lerp((density[1] - density[0]) * 0.5f + density[1], (density[2] - density[3]) * 0.5f + density[2], position.x);
					const vector vXLerp = lerp(vInner, vOuter, fBias.x);
					const float  vMajor = lerp(vXLerp[1], vXLerp[2], position.y);
					const float  vMinor = lerp((vXLerp[1] - vXLerp[0]) * 0.5f + vXLerp[1], (vXLerp[2] - vXLerp[3]) * 0.5f + vXLerp[2], position.y);

					const float result = lerp(vMajor, vMinor, fBias.y);

					vertices[x * 2 + y] = origin + float3(position, -result);
				}
			break;
		case 1:	// 01==Smoothstep
			[unroll]
			for(x = 0; x < 9; x++)
				[unroll]
				for(y = 0; y < 2; y++) {
					const float2 position = float2(x, y + strip) * 0.125f;

					const float2 fBias_ = (position - 0.5f);
					const float2 fBias  = (0.25f - (fBias_ * fBias_)) * 2.0f;

					const vector vInner = lerp(density[1], density[2], position.x);
					const vector vOuter = lerp((density[1] - density[0]) * 0.5f + density[1], (density[2] - density[3]) * 0.5f + density[2], position.x);
					const vector vXLerp = lerp(vInner, vOuter, fBias.x);
					const float  vMajor = lerp(vXLerp[1], vXLerp[2], position.y);
					const float  vMinor = lerp((vXLerp[1] - vXLerp[0]) * 0.5f + vXLerp[1], (vXLerp[2] - vXLerp[3]) * 0.5f + vXLerp[2], position.y);

					const float result = lerp(vMajor, vMinor, fBias.y);

					vertices[x * 2 + y] = origin + float3(position, -smoothstep(0.0f, 1.0f, result));
				}
			break;
		case 2:	// 02==Flatten top
			[unroll]
			for(x = 0; x < 9; x++)
				[unroll]
				for(y = 0; y < 2; y++) {
					const float2 position = float2(x, y + strip) * 0.125f;

					const float2 fBias_ = (position - 0.5f);
					const float2 fBias  = (0.25f - (fBias_ * fBias_)) * 2.0f;

					const vector vInner = lerp(density[1], density[2], position.x);
					const vector vOuter = lerp((density[1] - density[0]) * 0.5f + density[1], (density[2] - density[3]) * 0.5f + density[2], position.x);
					const vector vXLerp = lerp(vInner, vOuter, fBias.x);
					const float  vMajor = lerp(vXLerp[1], vXLerp[2], position.y);
					const float  vMinor = lerp((vXLerp[1] - vXLerp[0]) * 0.5f + vXLerp[1], (vXLerp[2] - vXLerp[3]) * 0.5f + vXLerp[2], position.y);

					const float result = lerp(vMajor, vMinor, fBias.y);

					vertices[x * 2 + y] = origin + float3(position, -min(1.0f, result));
				}
			break;
		case 3:	// 03==Flatten bottom
			[unroll]
			for(x = 0; x < 9; x++)
				[unroll]
				for(y = 0; y < 2; y++) {
					const float2 position = float2(x, y + strip) * 0.125f;

					const float2 fBias_ = (position - 0.5f);
					const float2 fBias  = (0.25f - (fBias_ * fBias_)) * 2.0f;

					const vector vInner = lerp(density[1], density[2], position.x);
					const vector vOuter = lerp((density[1] - density[0]) * 0.5f + density[1], (density[2] - density[3]) * 0.5f + density[2], position.x);
					const vector vXLerp = lerp(vInner, vOuter, fBias.x);
					const float  vMajor = lerp(vXLerp[1], vXLerp[2], position.y);
					const float  vMinor = lerp((vXLerp[1] - vXLerp[0]) * 0.5f + vXLerp[1], (vXLerp[2] - vXLerp[3]) * 0.5f + vXLerp[2], position.y);

					const float result = lerp(vMajor, vMinor, fBias.y);

					vertices[x * 2 + y] = origin + float3(position, -max(0.0f, result));
				}
			break;
		}
		break;
	case 1:	// Acute edge
		switch(method & 0x07) {
		case 0:	// 08==Shallow
			[unroll]
			for(x = 0; x < 9; x++)
				[unroll]
				for(y = 0; y < 2; y++) {
					const float2 position = float2(x, y + strip) * 0.125f;

					const float4 fPosInv = pow(float4(position, 1.0f - position), 6);
					const float2 fScaleX = float2((density[1].y <= density[2].y ? fPosInv.x : 1.0f - fPosInv.z), (density[1].z <= density[2].z ? fPosInv.x : 1.0f - fPosInv.z));
					const float2 fMinor  = lerp(density[1].yz, density[2].yz, fScaleX);
					const float  fScaleY = (fMinor.x <= fMinor.y ? fPosInv.y : 1.0f - fPosInv.w);
					const float  fMajor  = lerp(fMinor[0], fMinor[1], fScaleY);

					const float result = fMajor;

					vertices[x * 2 + y] = origin + float3(position, -result);
				}
			break;
		case 1:	// 09==Wide
			[unroll]
			for(x = 0; x < 9; x++)
				[unroll]
				for(y = 0; y < 2; y++) {
					const float2 position = float2(x, y + strip) * 0.125f;

					const float4 fPosInv = pow(float4(1.0f - position, position), 6);
					const float2 fScaleX = float2((density[1].y <= density[2].y ? 1.0f - fPosInv.x : fPosInv.z), (density[1].z <= density[2].z ? 1.0f - fPosInv.x : fPosInv.z));
					const float2 fMinor  = lerp(density[1].yz, density[2].yz, fScaleX);
					const float  fScaleY = (fMinor.x <= fMinor.y ? 1.0f - fPosInv.y : fPosInv.w);
					const float  fMajor  = lerp(fMinor[0], fMinor[1], fScaleY);

					const float result = fMajor;

					vertices[x * 2 + y] = origin + float3(position, -result);
				}
			break;
		}
		break;
	}

	return vertices;
}
