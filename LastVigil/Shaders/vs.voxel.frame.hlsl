/************************************************************
 * File: vs.voxel.frame.hlsl            Created: 2023/01/16 *
 * Type: Vertex shader            Last modified: 2023/01/17 *
 *                                                          *
 * Notes: 1)Add junction highlighting option                *
 *        2)Add plane highlight option                      *
 *                                                          *
 *  Copyright (c) David William Bull. All rights reserved.  *
 ************************************************************/

cbuffer CB_VIEW : register(b0) {   // 160 bytes (10 vectors)
   const matrix projection;   // Perspective transformation
   const matrix orthographic; // Orthographic transformation
   const float2 guiScale;     // Final X, Y scaling factors for GUI
   const uint2  bitField;     // 0-63==???
};

cbuffer CB_MAIN : register(b1) {   // 96 bytes (6 vectors)
   const matrix camera;     // Camera transformation
   const uint   frameCount; // Total number of frames presented
   const float  frameTime;  // Duration of last frame
   const float  secsDelta;  // == (Time elapsed - SecsTotal)
   const uint   st_bf;      // 0-23==secsTotal (Elapsed seconds), 24==Draw transparent sprites, 25-31==???
   const uint2  RES;
};

struct VOut {
   float4 pos : SV_Position;
   float2 tex : TEXCOORD;
   float  sec : TIME;
   uint   col : COLOUR;
};

VOut main(in const int3 pos : POSITION, in const uint col : COLOUR, const uint vID : SV_VertexID) {
   const matrix mTransform    = mul(camera, projection);
   const float3 vPosOS[18]    = { { 0.0f, 0.0f,-1.0f }, { 0.0f, 1.0f,-1.0f }, { 1.0f, 0.0f,-1.0f }, { 1.0f, 1.0f,-1.0f },
                                  { 1.0f, 0.0f, 0.0f }, { 1.0f, 1.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 1.0f, 0.0f },
                                  { 0.0f, 1.0f, 0.0f }, { 1.0f, 1.0f, 0.0f },
                                  { 1.0f, 1.0f, 0.0f }, { 1.0f, 1.0f,-1.0f }, { 0.0f, 1.0f, 0.0f }, { 0.0f, 1.0f,-1.0f },
                                  { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f,-1.0f }, { 1.0f, 0.0f, 0.0f }, { 1.0f, 0.0f,-1.0f } };
   const float2 vTexCoord[18] = { { 0.0f, 0.0f }, { 0.0f, 1.0f }, { 1.0f, 0.0f }, { 1.0f, 1.0f },
                                  { 0.0f, 0.0f }, { 0.0f, 1.0f }, { 1.0f, 0.0f }, { 1.0f, 1.0f },
                                  { 1.0f, 1.0f }, { 0.0f, 0.0f },
                                  { 0.0f, 0.0f }, { 0.0f, 1.0f }, { 1.0f, 0.0f }, { 1.0f, 1.0f },
                                  { 0.0f, 0.0f }, { 0.0f, 1.0f }, { 1.0f, 0.0f }, { 1.0f, 1.0f } };

   const float fTimeScale = abs((((float(st_bf & 0x0FFFFFF) + secsDelta) * 1.777777777777f) % 1.0f) * 2.0f - 1.0f);
   const VOut  output     = { mul(float4(float3(pos) + vPosOS[vID], 1.0f), mTransform), vTexCoord[vID], min(0.7071f, 1.0f - (fTimeScale * fTimeScale * fTimeScale)) + 1.0f, col };
   
   return output;
}
