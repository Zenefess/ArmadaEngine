/************************************************************
 * File: vs.pointquad.hlsl              Created: 2020/10/27 *
 *                                Last modified: 2020/10/30 *
 *                                                          *
 * Desc:                                                    *
 *                                                          *
 *  Copyright (c) David William Bull. All rights reserved.  *
 ************************************************************/

struct VOut {
	float3 trans : POSITION;
	float3 r_s : NORMAL;
	uint   bits : BLENDINDICES;
};

/*  Reduced packet size -- 256x256 max atlas segments
	 -------------------
FLOAT3	Sprite location				// 12 bytes
FLOAT		Rotation							// 20 bytes
FLOAT2	Scale								// 24 bytes
UINT											// 28 bytes
	FIXED8p0		Texture X segment
	FIXED8p0		Texture X total segments
	FIXED8p0		Texture Y segment
	FIXED8p0		Texture Y total segments
*/

VOut SquadLineX(in VOut ia) {
	VOut vo;
	
	
	
	return vo;
}

VOut SquadLineY(in VOut ia) {
	VOut vo;
	
	
	
	return vo;
}

VOut SquadVFormation(in VOut ia) {
	VOut vo;
	
	
	
	return vo;
}

VOut SquadTriangle(in VOut ia) {
	VOut vo;
	
	
	
	return vo;
}

VOut SquadQuad(in VOut ia) {
	VOut vo;
	
	
	
	return vo;
}

VOut SquadCircle(in VOut ia) {
	VOut vo;
	
	
	
	return vo;
}

VOut main(in VOut ia) {
	VOut vo;
	 
	vo.trans = ia.trans;
	vo.r_s = ia.r_s;
	vo.bits = ia.bits;
	


	return vo;
}
