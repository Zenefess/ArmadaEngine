/************************************************************
 * File: gs.instanced.sprites.hlsl      Created: 2022/11/21 *
 *                                    Last mod.: 2022/12/07 *
 *                                                          *
 * Desc: Geometry shader. Generates 3-space sprites from    *
 *       point lists. Animation support, optional billboard *
 *       orienting, and choice between tri and quad.        *
 *                                                          *
 *  Copyright (c) David William Bull. All rights reserved.  *
 ************************************************************/

#include "common.hlsli"

cbuffer CB_VIEW : register(b0) {   // 144 bytes (9 vectors)
   cmatrix projection;   // Perspective transformation
   cmatrix orthographic; // Orthographic transformation
   cfloat2 guiScale;     // Final X, Y scaling factors for GUI
   cuint2  bitField;     // 0~63==???
};

cbuffer CB_MAIN : register(b1) {   // 80 bytes (5 vectors)
   cmatrix camera;     // Camera transformation
   cuint   frameCount; // Total number of frames presented
   cfloat  frameTime;  // Duration of last frame
   cfloat  secsDelta;  // == (Time elapsed - SecsTotal)
   cuint   st_bf;      // 0~23==secsTotal (Elapsed seconds), 24==Draw transparent sprites, 25-31==???
};

struct OBJECT_IMM {   // (1 scalar)
   uint bits; // 0-4==Quad count, 5~7==???, 8-31==PART_IMM starting index
};

struct PART_IMM {   // (5 vectors)
   float3 pos;     // Position (relative to origin)
   uint   ai_bits; // 0~7==Runtime index of atlas texture, 8==Shape (quad/tri), 9==Billboard, 10=31==???
   float3 rot;     // Rotation (relative to origin)
   uint   RES1;
   float2 size;    // Relative to parent
   uint2  tc;      // tex[0] == Diffuse map | tex[2].x == Paint map    | tex[3].x == Highlight map
                   // tex[1] == Normal map  | tex[2].y == Emission map | tex[3].y == Occlusion map
   float3 t_pos;   // Slide vector (maximum transformation)
   uint   RES2;      
   float3 t_rot;   // Rotation vector (maximum transformation)
   uint   RES3;      
};

struct BONE_DYN {   // (3 scalars)
   float3 pos;
   float  lerp;    // pos->mt.s & rot->mt.r (current recoil state)
   float3 rot;
   uint   afc_ft;  // 0~23==Animation frame time : 16p8, 24~31==Animation frame count
   float3 size;    // Scale (.x==0 ? Not drawn)
   uint   afo_sai; // 0~23==SPRITE array index (~16M unique), 24~31==Animation frame offset
};

struct GOut {   // 56 bytes (3 vectors + 2 scalars)
   float4 pos      : SV_Position;
   float3 position : POSITION;
   uint   si_ai    : SPRITEINDEX;
   uint3  rot      : ROTATIONS;   // sin & cos of XYZ rotations
   uint   RES      : RESERVED;
   float2 tex      : TEXCOORD;
};

const StructuredBuffer<uint4> object;
const StructuredBuffer<PART_IMM> part;
      StructuredBuffer<BONE_DYN> bone;

// index[0]: 0-11==Object index (~4K unique), 12-31==Parent's bone index (~1,024K unique)
[instance(32)] [maxvertexcount(4)]
void main(point cuint index[1] : INDEX, cuint iID : SV_GSInstanceID, inout TriangleStream<GOut> triStream) {
   GOut     output;
   float2x3 fRot;
   // Abandon quad if infinitely small
   if(bone[index[0] >> 12].size.x == 0.0f) return;
   // Unpack object array index from input data
   cuint uiOAI = index[0] & 0x0FFF;
   // Unpack quad count from object header bits
   cuint uiQuadC = (object[uiOAI >> 2][uiOAI & 0x03] & 0x01F) - iID;
   // Abandon quad if part is non-existant
   if(uiQuadC < 0) return;
   // Unpack array indices
   cuint uiBPI = index[0] >> 12;
   cuint uiBAI = uiBPI + uiQuadC;
   cuint uiSAI = bone[uiBAI].afo_sai & 0x0FFFFFF;
   cuint uiPPI = object[uiOAI >> 2][uiOAI & 0x03] >> 8;
   cuint uiPAI = uiPPI + uiQuadC;
   // Unpack bone bits
   cfloat fAFO = float((bone[uiBAI].afo_sai >> 24) & 0x0FF);
   cfloat fAFC = float(((bone[uiBAI].afc_ft >> 24) & 0x0FF) + 1);
   cfloat fFT  = float(bone[uiBAI].afc_ft & 0x0FF) * 0.00390625f + float((bone[uiBAI].afc_ft >> 8) & 0x0FFFF);
   // Unpack part bits
   cbool bShape     = (part[uiPAI].ai_bits >> 8) & 0x01;
   cbool bBillboard = (part[uiPAI].ai_bits >> 9) & 0x01;
   // Calculate first half of size
   float3 fVert1 = bone[uiBAI].size * float3(part[uiPAI].size, 0.0f);
   // Calculate adjusted rotation
   float3 fRotation = lerp(part[uiPPI].rot + bone[uiBPI].rot, part[uiPPI].t_rot + bone[uiBPI].rot, bone[uiBPI].lerp);
   float3 fPosition = 0.0f;
   // Transrotate if child
   if(uiQuadC) {
      fVert1 *= bone[uiBPI].size;
      // Calculate adjusted position
      fPosition = lerp(part[uiPAI].pos + bone[uiBAI].pos, part[uiPAI].t_pos + bone[uiBAI].pos, bone[uiBAI].lerp);
      fPosition *= float3(part[uiPPI].size, 1.0f) * bone[uiBPI].size;
      sincos(fRotation, fRot[0], fRot[1]);
      fPosition.xy = RotateVector(fPosition.xy, fRot._23_13.xy);
      fPosition.xz = RotateVector(fPosition.xz, fRot._22_12.xy);
      fPosition.yz = RotateVector(fPosition.yz, fRot._21_11.xy);
      // Adjust rotation offset
      fRotation += lerp(part[uiPAI].rot + bone[uiBAI].rot, part[uiPAI].t_rot, bone[uiBAI].lerp);
   }
   fPosition += lerp(part[uiPPI].pos + bone[uiBPI].pos, part[uiPPI].t_pos + bone[uiBPI].pos, bone[uiBPI].lerp);
   // Calculate second half of size
   float3 fVert2 = float3(-fVert1.x, fVert1.yz);
   // Unpack texture coordinates
   cfloat4 fTC = float4((part[uiPAI].tc[0] >> uint2(0, 16)) & 0x0FFFF, (part[uiPAI].tc[1] >> uint2(0, 16)) & 0x0FFFF) * 0.000030517578125f;
   // Rotate
   sincos(fRotation, fRot[0], fRot[1]);
   fVert1.xy = RotateVector(fVert1.xy, fRot._23_13.xy);
   fVert2.xy = RotateVector(fVert2.xy, fRot._23_13.xy);
   // Rotate around X and Y if not a billboard
   if(!bBillboard) {
      fVert1.xz = RotateVector(fVert1.xz, fRot._22_12.xy);
      fVert2.xz = RotateVector(fVert2.xz, fRot._22_12.xy);
      fVert1.yz = RotateVector(fVert1.yz, fRot._21_11.xy);
      fVert2.yz = RotateVector(fVert2.yz, fRot._21_11.xy);
   }
   // Calculate animation frame offset
   //cfloat fFrameOS = (fTC.z - fTC.x) * trunc(((Seconds(secsTotal, secsDelta) / fFT) + fAFO) % fAFC);
   cuint  secsTotal = st_bf & 0x0FFFFFF;
   cfloat fFTDelta  = float(Seconds(secsTotal, secsDelta) * double(rcp(fFT)));
   cfloat fFrameOS  = (fTC.z - fTC.x) * trunc((fFTDelta + fAFO) % fAFC);
   
   sincos(fRotation, fRot[0], fRot[1]);
   output.si_ai = uiSAI | ((part[uiPAI].ai_bits & 0x07F) << 25);
   output.rot = f32tof16(fRot[1]) | (f32tof16(fRot[0]) << 16);
   output.RES = 0;
   // Bottom-left vertex
   output.position = fPosition;
   if(!bBillboard) output.position -= fVert1;
   float4 prePos = mul(float4(output.position, 1.0f), camera);
   if(bBillboard) prePos -= float4(fVert1, 0.0f);
   output.pos = mul(prePos, projection);
   output.tex = float2(fTC.x + fFrameOS, fTC.w);
   triStream.Append(output);
   // Top(-left) vertex
   output.position = fPosition;
   if(!bBillboard && !bShape) output.position += fVert2;   // Shift to top-left if quadrilateral
   prePos = mul(float4(output.position, 1.0f), camera);
   if(bBillboard && !bShape) prePos += float4(fVert2, 0.0f);
   output.pos = mul(prePos, projection);
   output.tex = float2(fTC.x + fFrameOS, fTC.y);
   triStream.Append(output);
   // Bottom-right vertex
   output.position = fPosition;
   if(!bBillboard) output.position -= fVert2;
   prePos = mul(float4(output.position, 1.0f), camera);
   if(bBillboard) prePos -= float4(fVert2, 0.0f);
   output.pos = mul(prePos, projection);
   output.tex = float2(fTC.z + fFrameOS, fTC.w);
   triStream.Append(output);
   // Top-right vertex
   if(bShape) return;   // Exit if triangle
   output.position = fPosition;
   if(!bBillboard) output.position += fVert1;
   prePos = mul(float4(output.position, 1.0f), camera);
   if(bBillboard) prePos += float4(fVert1, 0.0f);
   output.pos = mul(prePos, projection);
   output.tex = float2(fTC.z + fFrameOS, fTC.y);
   triStream.Append(output);
}
