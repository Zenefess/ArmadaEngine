/************************************************************
 * File: Meshes.h                       Created: 2022/10/20 *
 *                                Last modified: 2022/10/20 *
 *                                                          *
 * Desc:                                                    *
 *                                                          *
 *  Copyright (c) David William Bull. All rights reserved.  *
 ************************************************************/

#pragma once

struct al4 VERTEX1 {
/* Index bitpattern : VVVVVVVUUUUUUUYYYYYYYXXXXXXX..II
	00-01 . Vertex index (0-3)
	02-03 . ??
	04-10 . X divisions of atlas texture (0-127)
	11-17 . Y divisions of atlas texture (0-127)
	18-24 . Quad index, U (0-127)
	25-31 . Quad index, V (0-127)
*/	ui32 b;
};

struct al4 VERTEX21 {
	float u, v;
	ui32	i;
};

struct al4 VERTEX33 {
	float x, y, z,
			r, sx, sy;
};

struct al4 VERTEX331 {
	float x, y, z,
			r, sx, sy;
	ui8	t[4];
};

struct al4 VERTEX3311 {
	float x, y, z,
			r, sx, sy;
	ui8	t[4];
	ui32	b;
};

struct al4 VERTEX33311 {
	float x, y, z,
			x0, y0, z0,
			r, sx, sy;
	ui8	t[4];
	ui32	b;
};

struct al4 VERTEX332 {
	float x, y, z,
			r, sx, sy,
			u, v;
};

struct al4 VERTEX3322 {
	float x, y, z,
			r, sx, sy,
			u1, v1, u2, v2;
};

const al16 struct QUAD {
	VERTEX332 v[4];
};

const al16 struct QUADi {
	VERTEX332 v[4];
	al16 ui16 i[4];
};

const al16 struct QUAD1i {
	VERTEX332 v[4];
	al16 ui16 i[6];
};

const al16 struct QUAD2i {
	VERTEX332 v[8];
	al16 ui16 i[8];
};

const al16 struct CUBEi {
	VERTEX332 v[8];
	al16 ui16 i[14];
};

const al16 QUADi mQuad = {
	{
		{-0.5f,-0.5f, 0.0f, 0.0f, 0.0f,-1.0f, 0.0f, 1.0f},
		{-0.5f, 0.5f, 0.0f, 0.0f, 0.0f,-1.0f, 0.0f, 0.0f},
		{ 0.5f,-0.5f, 0.0f, 0.0f, 0.0f,-1.0f, 1.0f, 1.0f},
		{ 0.5f, 0.5f, 0.0f, 0.0f, 0.0f,-1.0f, 1.0f, 0.0f}
	},
};

const al16 QUADi miQuad = {
	{
		{-0.5f,-0.5f, 0.0f, 0.0f, 0.0f,-1.0f, 0.0f, 1.0f},
		{ 0.5f,-0.5f, 0.0f, 0.0f, 0.0f,-1.0f, 1.0f, 1.0f},
		{-0.5f, 0.5f, 0.0f, 0.0f, 0.0f,-1.0f, 0.0f, 0.0f},
		{ 0.5f, 0.5f, 0.0f, 0.0f, 0.0f,-1.0f, 1.0f, 0.0f}
	},
	{
		0, 2, 1, 3
	}
};

const al16 QUAD1i miQuad1 = {
	{
		{-0.5f,-0.5f, 0.0f, 0.0f, 0.0f,-1.0f, 0.0f, 1.0f},
		{ 0.5f,-0.5f, 0.0f, 0.0f, 0.0f,-1.0f, 1.0f, 1.0f},
		{-0.5f, 0.5f, 0.0f, 0.0f, 0.0f,-1.0f, 0.0f, 0.0f},
		{ 0.5f, 0.5f, 0.0f, 0.0f, 0.0f,-1.0f, 1.0f, 0.0f}
	},
	{
		0, 2, 1, 3, 0, 2
	}
};

const al16 QUAD2i miQuad2 = {
	{
		{-0.5f,-0.5f, 0.0f, 0.0f, 0.0f,-1.0f, 0.0f, 1.0f},
		{ 0.5f,-0.5f, 0.0f, 0.0f, 0.0f,-1.0f, 1.0f, 1.0f},
		{-0.5f, 0.5f, 0.0f, 0.0f, 0.0f,-1.0f, 0.0f, 0.0f},
		{ 0.5f, 0.5f, 0.0f, 0.0f, 0.0f,-1.0f, 1.0f, 0.0f},
		{-0.5f,-0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f},
		{ 0.5f,-0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f},
		{-0.5f, 0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f},
		{ 0.5f, 0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f}
	},
	{
		0, 2, 1, 3, 5, 7, 4, 6
	}
};

const CUBEi miCube = {
	{
		{-0.5f,-0.5f,-0.5f,-0.57735f,-0.57735f,-0.57735f, 0.0f, 1.0f},
		{ 0.5f,-0.5f,-0.5f, 0.57735f,-0.57735f,-0.57735f, 1.0f, 1.0f},
		{-0.5f, 0.5f,-0.5f,-0.57735f, 0.57735f,-0.57735f, 0.333333f, 0.666667f},
		{ 0.5f, 0.5f,-0.5f, 0.57735f, 0.57735f,-0.57735f, 0.666666f, 0.666667f},
		{-0.5f,-0.5f, 0.5f,-0.57735f,-0.57735f, 0.57735f, 0.0f, 0.0f},
		{ 0.5f,-0.5f, 0.5f, 0.57735f,-0.57735f, 0.57735f, 1.0f, 0.0f},
		{-0.5f, 0.5f, 0.5f,-0.57735f, 0.57735f, 0.57735f, 0.666667f, 0.333333f},
		{ 0.5f, 0.5f, 0.5f, 0.57735f, 0.57735f, 0.57735f, 0.333333f, 0.333333f}
	},
	{
		0, 2, 1, 3, 7, 2, 6,
		4, 7, 5, 1, 4, 0, 2
	}
};

struct al8 VEC2D {
	union {
		float a, w, x;
	};
	union {
		float b, h, y;
	};
};

struct al8 VEC3D {
	union {
		float a, w, x;
	};
	union {
		float b, h, y;
	};
	union {
		float c, d, z;
	};
};

struct al2 I82D { ui8 x, y; };

struct al4 I162D { ui16 x, y; };

struct al4 I8COL { ui8 r, g, b, a; };

struct al4 TEXSEGMENT { ui8 xSeg, xDivs, ySeg, yDivs; };

struct al16 TEXCOORDS { float u1, v1, u2, v2; };

struct MATERIAL1 {	// 12 bytes
	TEXSEGMENT	ts;
	I8COL			tint;
	si16			emm,			// Emission (ambient light value) Fixed8p8s
					tex;
};

struct MATERIAL2 {	// 24 bytes
	TEXCOORDS	co;
	I8COL			tint;
	si16			emm,			// Emission (ambient light value) Fixed8p8s
					tex;
};

struct MATERIAL3 {	// 60 bytes
	TEXCOORDS	co,
					co2,
					co3;
	I8COL			tint;
	si16			emm,			// Emission (ambient light value) Fixed8p8s
					tex1,
					tex2,
					tex3;
};

struct SPRITE1 {	// 20 bytes
	VEC2D			size;
	MATERIAL1	mat;
};

struct SPRITE2 {	// 32 bytes
	VEC2D			size;
	MATERIAL2	mat;
};

struct SPRITE3 {	// 68 bytes
	VEC2D			size;
	MATERIAL3	mat;
};

struct NEUROLOGY {  // 6 bytes
	ui8			thoughtsPerSec,
					thoughtLatency,
					thoughtsSimMax,
					thoughtDisrupt,
					decisionDisrupt,
					decisionConf;
};

struct al16 BRAIN {	// 12 bytes
	ui64			statusFlags;
	si16			psychIndex;
	si16			neuroIndex;
};

struct al16 SKELETON {
	VEC3D			loc;
	VEC3D			dest;
	// ???
 };

struct al16 ENTITY {  // 32 bytes
	BRAIN			*brain;
	SKELETON		*body;
	union {
		SPRITE1 *s1;
		SPRITE2 *s2;
		SPRITE3 *s3;
	};
	ui8			spriteType;
	ui8			RES[7];
};