/************************************************************
 * File: vs.autosprite.hlsl             Created: 2020/10/25 *
 *                                Last modified: 2020/10/26 *
 *                                                          *
 * Desc:                                                    *
 *                                                          *
 *  Copyright (c) David William Bull. All rights reserved.  *
 ************************************************************/

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
    
// Index bitpattern: YYYYYYYYYYYYYYYXXXXXXXXXXXXXXXVV
//
// 00-01 .  uint2b   == Vertex index (0-3)
// 02-17 . fixed1p14 == X offset into atlas texture(s)
// 18-31 . fixed1p14 == Y offset into atlas texture(s)
//
VOut main(uint bitField : BLENDINDICES) {
    VOut output;
    
    const uint   i        = bitField & 0x03u;
    const uint   xStep    = (bitField >> 2) & 0x7FFFu;
    const uint   yStep    = (bitField >> 17);
    const float  xDelta   = float(xStep) * 0.00006103515625f;
    const float  yDelta   = float(yStep) * 0.00006103515625f;
    const float2 texCoord = float2(xDelta, yDelta);
    
    output.position = (float3) mul(float4(position[i], 1.0f), model);
    output.n = mul(float3(0.0f, 1.0f, 0.0f), (float3x3) model);
    output.texcoord = texCoord;
    output.pos = mul(float4(output.position, 1.0f), modelViewProj);
    
    return output;
}
