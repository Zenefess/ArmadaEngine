/**********************************************************************
 * File: gs.entities.sprites.x32.hlsl             Created: 2023/06/10 *
 * Type: Geometry shader                    Last modified: 2023/06/11 *
 *                                                                    *
 *                                                                    *
 * Desc: Generates 3-space sprites from point lists.                  *
 *       Animation support, optional billboard orienting, and choice  *
 *       between tri and quad.                                        *
 *       32 sprites per instance.                                     *
 *                                                                    *
 * Copyright (c) David William Bull.             All rights reserved. *
 **********************************************************************/

#include "common.hlsli"

cbuffer CB_VIEW : register(b0) { // 160 bytes (10 vectors)
   const matrix projection;   // Perspective transformation
   const matrix orthographic; // Orthographic transformation
   const float2 guiScale;     // Final X & Y scaling factors for GUI
   const uint2  bitField;     // 0-63==???
   const uint4  RES;
};

cbuffer CB_MAIN : register(b1) { // 96 bytes (6 vectors)
   const matrix camera;     // Camera transformation
   const uint   frameCount; // Total number of frames presented
   const float  frameTime;  // Duration of last frame
   const float  secsDelta;  // == (Time elapsed - SecsTotal)
   const uint   st_bf;      // 0-23==secsTotal (Elapsed seconds), 24==Draw transparent sprites, 25-31==???
   const uint4  RES2;
};

struct OBJECT_IGS { // 8 bytes (2 scalar)
   uint ppi;     // Parent PART_IGS index
   uint qc_bits; // 0~7==Part count - 1 (~256), 6~31==Bitfield
};

struct PART_IGS { // 80 bytes (5 vectors)
   float3 pos;     // Relative position
   uint   ai_bits; // 0~7==Runtime index of atlas texture, 8==Shape (quad/tri), 9==Billboard, 10=31==???
   float3 rot;     // Relative rotation
   uint   oppi;    // Owner's parent part index (this==No owner)
   float2 size;    // Relative to parent
   uint2  tc;      // tex[0] == Diffuse map | tex[2].x == Paint map    | tex[3].x == Highlight map
                   // tex[1] == Normal map  | tex[2].y == Emission map | tex[3].y == Occlusion map
   float3 t_pos;   // Slide vector (maximum transformation)
   uint   RES1;      
   float3 t_rot;   // Rotation vector (maximum transformation)
   uint   RES2;      
};

struct BONE_DGS { // 64 bytes (4 vectors)
   float3 pos;      // Current position (relative to owner or world)
   float  lerp;     // pos->mt.s & rot->mt.r (current recoil state)
   float3 rot;      // Current rotation (relative to owner or world)
   float  aft;      // Animation frame time
   float3 size;     // Scale (.x==0 ? Not drawn)
   uint   afco_oai; // 0~7==Animation frame count - 1, 8~15==Animation frame offset, 16~31==Object array index
   uint   opi;      // Object part index
   uint   sai;      // SPRITE array index
   uint   pbi;      // Parent's bone index (this==No owner)
   uint   obi;      // Owner's bone index (this==No owner)
};

struct GOut { // 56 bytes (3 vectors + 2 scalars)
   float4 pos      : SV_Position;
   float3 position : POSITION;
   uint   si       : SPRITEINDEX;
   uint3  rot      : ROTATIONS;   // sin & cos of XYZ rotations
   uint   ai       : ATLASINDEX;  // 0~6==Atlas index
   float2 tex      : TEXCOORD;
};

const StructuredBuffer <OBJECT_IGS> object;
const StructuredBuffer <PART_IGS>   part;
const StructuredBuffer <BONE_DGS>   bone;

// index[0]: uint4x8==32 bone indices
[instance(32)] [maxvertexcount(4)]
void main(point const uint4x8 index[1] : INDEX, const uint iID : SV_GSInstanceID, inout TriangleStream<GOut> triStream) {
   const uint uiBAI = index[0][iID >> 2][iID & 0x03];
   GOut       output;
   float2x3   fRot;
   // Abandon sprite if infinitely small
   if(bone[uiBAI].size.x == 0.0f) return;
   // Unpack object array index from input data
   const uint uiOAI = bone[uiBAI].afco_oai >> 16;
   // cache array indices
   const uint uiBPI = bone[uiBAI].pbi;
   const uint uiOBI = bone[uiBAI].obi;
   const uint uiPPI = object[uiOAI].ppi;
   const uint uiPAI = bone[uiBAI].opi;
   // Unpack part bits
   const bool bShape     = (part[uiPAI].ai_bits >> 8) & 0x01;
   const bool bBillboard = (part[uiPAI].ai_bits >> 9) & 0x01;
   // Calculate first half of size
   float3 fVert1 = bone[uiBAI].size * float3(part[uiPAI].size, 0.0f);
   // Calculate adjusted rotation
   float3 fRotation = lerp(part[uiPPI].rot + bone[uiBPI].rot, part[uiPPI].t_rot + bone[uiBPI].rot, bone[uiBPI].lerp);
   float3 fPosition = 0.0f;
   // Transrotate if child bone
   if(uiBAI != uiBPI) {
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
   // Transrotate if owned
   if(uiBPI != uiOBI) {
      const uint ownerPAI = bone[uiOBI].opi;
      const uint ownerPBI = bone[uiOBI].pbi;

      fVert1 *= bone[ownerPBI].size;
      // Calculate adjusted position
      fPosition += lerp(part[ownerPAI].pos + bone[uiOBI].pos, part[ownerPAI].t_pos + bone[uiOBI].pos, bone[uiOBI].lerp);
      fPosition *= float3(part[part[ownerPAI].oppi].size, 1.0f) * bone[ownerPBI].size;
      sincos(fRotation, fRot[0], fRot[1]);
      fPosition.xy = RotateVector(fPosition.xy, fRot._23_13.xy);
      fPosition.xz = RotateVector(fPosition.xz, fRot._22_12.xy);
      fPosition.yz = RotateVector(fPosition.yz, fRot._21_11.xy);
      // Adjust rotation offset
      fRotation += lerp(part[ownerPAI].rot + bone[uiOBI].rot, part[ownerPAI].t_rot, bone[uiOBI].lerp);
   }
   // Calculate second half of size
   float3 fVert2 = float3(-fVert1.x, fVert1.yz);
   // Unpack texture coordinates
   const float4 fTC = float4((part[uiPAI].tc[0] >> uint2(0, 16)) & 0x0FFFF, (part[uiPAI].tc[1] >> uint2(0, 16)) & 0x0FFFF) * 0.000030517578125f;
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
   // Unpack frame time values
   const float fAFC = float((bone[uiBAI].afco_oai & 0x0FF) + 1);
   const float fAFO = float((bone[uiBAI].afco_oai >> 8) & 0x0FF);
   // Calculate animation frame offset
   const uint  secsTotal = st_bf & 0x0FFFFFF;
   const float fFTDelta  = float(Seconds(secsTotal, secsDelta) * double(rcp(bone[uiBAI].aft)));
   const float fFrameOS  = (fTC.z - fTC.x) * trunc((fFTDelta + fAFO) % fAFC);
   
   sincos(fRotation, fRot[0], fRot[1]);
   output.si = bone[uiBAI].sai;
   output.ai = part[uiPAI].ai_bits & 0x07F;
   output.rot = f32tof16(fRot[1]) | (f32tof16(fRot[0]) << 16);
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
