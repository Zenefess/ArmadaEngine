/************************************************************
 * File: class_entitymanager.h          Created: 2023/05/06 *
 *                                Last modified: 2023/05/08 *
 *                                                          *
 * Desc:                                                    *
 *                                                          *
 *  Copyright (c) David William Bull. All rights reserved.  *
 ************************************************************/
#pragma once

#include "pch.h"
#include "Data structures.h"
#include "Entity structures.h"
#include "Common functions.h"

#define MAX_OBJECT_GROUPS    64
#define MAX_ENTITY_GROUPS    64

#define MAX_OBJECTS          4096
//#define MAX_PARTS_PER_OBJECT 32
#define MAX_PARTS            32768
#define MAX_ENTITIES         1048576
#define MAX_BONES_PER_ENTITY 32
#define MAX_BONES            1048576
#define MAX_OPAQUE_SPRITES   1048576
#define MAX_TRANS_SPRITES    1048576

typedef cENTMAN_THREAD_DATA * const cEMTDptr;

vui128 ENTMAN_THREAD_STATUS = {};
static void _ET_Cull_Nonvisible_and_Unchanged(ptr);
static void _ET_Cull_Nonvisible_Accurate(ptr);
static void _ET_Cull_Unchanged(ptr);

inline void transrotate(BONE_DGS &bone, cfl32 dist) { bone.pos.x -= dist * sinf(bone.rot.z); bone.pos.y += dist * cosf(bone.rot.z); }

al32 struct CLASS_ENTITYMANAGER {
	OBJECT_GROUP         objGroup[MAX_OBJECT_GROUPS] {};
	ENTITY_GROUP         entGroup[MAX_ENTITY_GROUPS] {};

	ENTMAN_THREAD_DATA  *threadData = (ENTMAN_THREAD_DATA *)zalloc32(sizeof(ENTMAN_THREAD_DATA) * 2);

	si32 siObjects[MAX_OBJECT_GROUPS] {}, siParts[MAX_OBJECT_GROUPS] {};
	si32 siEntities[MAX_ENTITY_GROUPS] {}, siBones[MAX_ENTITY_GROUPS] {};
	si32 siSpritesO[MAX_ENTITY_GROUPS] {}, siSpritesT[MAX_ENTITY_GROUPS] {};
	si32 siEntityGroups = 0, siObjectGroups = 0, siEntry = 0;

#ifdef AE_PTR_LIB
	CLASS_ENTITYMANAGER(void) { ptrLib[3] = this; }
#endif

	inline cENT_PTRS const Pointers(csi16 objectGroup, csi16 entityGroup) const {
		return { objGroup[objectGroup].object, objGroup[objectGroup].part,
					entGroup[entityGroup].bone_dgs, entGroup[entityGroup].spriteO, entGroup[entityGroup].spriteT };
	}

	// Create array of object template slots
	cui32 CreateObjectGroup(wchptrc name, wchptrc text, csi32 maxObjects, csi32 maxParts) {
		// Find next available index
		for(siEntry = 0; objGroup[siEntry].object; siEntry++);
		if(siEntry >= MAX_OBJECT_GROUPS) { return 0x080000001; }
		if(maxObjects > MAX_OBJECTS) { return 0x080000002; }
		if(maxParts > MAX_PARTS) { return 0x080000003; }

		objGroup[siEntry] = { name, text, (OBJECT_IGS *)zalloc32(sizeof(OBJECT_IGS) * maxObjects),
									 (PART_IGS *)zalloc32(sizeof(PART_IGS) * maxParts), 0, 0, maxObjects, maxParts };
		siObjectGroups++;

		return siEntry;	// Return group index
	}

	// Create array of entity slots
	cui32 CreateEntityGroup(wchptrc name, wchptrc text, csi32 maxEntities, csi32 maxBones) {
		// Find next available index
		for(siEntry = 0; entGroup[siEntry].entity; siEntry++);
		if(siEntry >= MAX_ENTITY_GROUPS) { return 0x080000001; }
		if(maxEntities > MAX_ENTITIES) { return 0x080000002; }
		if(maxBones > MAX_BONES) { return 0x080000003; }

		entGroup[siEntry].name = name;
		entGroup[siEntry].text = text;
		entGroup[siEntry].entity = (ENTITY *)zalloc32(sizeof(ENTITY) * maxEntities);
		entGroup[siEntry].bone = (BONE *)zalloc32(sizeof(BONE) * maxBones);
		entGroup[siEntry].bone_dgs = (BONE_DGS *)zalloc32(sizeof(BONE_DGS) * maxBones);
		entGroup[siEntry].spriteO = (SPRITE_DPS *)zalloc32(sizeof(SPRITE_DPS) * maxBones);
		entGroup[siEntry].spriteT = (SPRITE_DPS *)zalloc32(sizeof(SPRITE_DPS) * maxBones);
		entGroup[siEntry].entityVis = (ui64ptr)zalloc32(maxEntities >> 3);
		entGroup[siEntry].entityMod = (ui64ptr)zalloc32(maxEntities >> 3);
		entGroup[siEntry].totalEntities = 0;
		entGroup[siEntry].totalBones = 0;
		entGroup[siEntry].totalBone_DGS = 0;
		entGroup[siEntry].totalSpritesO = 0;
		entGroup[siEntry].totalSpritesT = 0;
		entGroup[siEntry].maxEntities = maxEntities;
		entGroup[siEntry].maxBones = maxBones;
		siEntityGroups++;

		return siEntry;	// Return group index
	}

	// Remove group of object template slots. Returns number of remaining groups, or err01 if group was empty
	cui32 DestroyObjectGroup(csi16 group) {
		if(objGroup[group].object) {
			_aligned_free(objGroup[group].part);
			_aligned_free(objGroup[group].object);
			memset(&objGroup[group], 0, sizeof(OBJECT_GROUP));

			return --siObjectGroups;
		}

		return 0x80000001;
	}

	// Remove group of entity slots
	cui32 DestroyEntityGroup(csi16 group) {
		if(entGroup[group].entity) {
			_aligned_free(entGroup[siEntry].entityMod);
			_aligned_free(entGroup[siEntry].entityVis);
			_aligned_free(entGroup[group].spriteT);
			_aligned_free(entGroup[group].spriteO);
			_aligned_free(entGroup[group].bone);
			_aligned_free(entGroup[group].entity);
			memset(&objGroup[group], 0, sizeof(OBJECT_GROUP));

			return --siEntityGroups;
		}

		return 0x80000001;
	}

	cID32 CreateObject(cui16 group, cVEC3Df position, cVEC3Df orientation, cVEC2Df size, cVEC6Df recoilState, cVEC4Df texCoords, cui8 atlasIndex, csi8 partCount) {
		cui16 index = objGroup[group].totalObjects++;

		if(index >= MAX_OBJECTS) return { 0x08000, 0x01 };

		OBJECT_GROUP &curGroup = objGroup[group];

		csi32 partIndex = curGroup.totalParts;
		csi32 lastIndex = (curGroup.totalParts += partCount);

		curGroup.object[index].ppi = partIndex;
		curGroup.object[index].qc = partCount - 1;

		PART_IGS &curPart = curGroup.part[partIndex];

		curGroup.part[partIndex] = { position, atlasIndex, orientation, NULL, size, Fix16x4(texCoords, 32768.0f), { recoilState.pos, NULL, recoilState.ori } };
/*		curPart.pos     = position;
		curPart.ai_bits = atlasIndex;
		curPart.rot     = orientation;
		curPart.size    = size;
		curPart.tc      = Fix16x4(texCoords, 32768.0f);
		curPart.trans   = { recoilState.pos, NULL, recoilState.ori };*/

		for(ui8 i = partIndex + 1; i < lastIndex; i++) {
			curGroup.part[i] = { { 0.0f, 0.0f, 0.0f }, atlasIndex, { 0.0f, 0.0f, 0.0f }, NULL, { 1.0f, 1.0f },
										Fix16x4(texCoords, 32768.0f), { { 0.0f, 0.0f, 0.0f }, NULL, { 0.0f, 0.0f, 0.0f } } };
/*			part[group][i].pos     = { 0.0f, 0.0f, 0.0f };
			part[group][i].ai_bits = atlasIndex;
			part[group][i].rot     = { 0.0f, 0.0f, 0.0f };
			part[group][i].size    = { 1.0f, 1.0f };
			part[group][i].tc      = Fix16x4(texCoords, 32768.0f);
			part[group][i].trans   = { { 0.0f, 0.0f, 0.0f }, NULL, { 0.0f, 0.0f, 0.0f } };*/
		}

		return { index, group };
	}

	cID64 CreateEntity(cui32 group, ID32 object, cbool transparent) {
		cui32 index = entGroup[group].totalEntities++;

		if(index >= MAX_ENTITIES) return { 0x080000001, 0x0 };

		const OBJECT_GROUP &srcGroup = objGroup[object.group];
				ENTITY_GROUP &curGroup = entGroup[group];

		csi32 partCount = (srcGroup.object[object.index].bits >> 28) + 1;

		csi32 boneIndex = curGroup.totalBones;
		csi32 lastIndex = (curGroup.totalBones += partCount);

		curGroup.entity[index].part[0]     = &curGroup.entity[index];
		curGroup.entity[index].bone        = &curGroup.bone[boneIndex];
		curGroup.entity[index].geometry	  = &curGroup.bone_dgs[boneIndex];
		curGroup.entity[index].numParts    = partCount;
		curGroup.entity[index].objectIndex = object.index;
		curGroup.entity[index].boneIndex   = boneIndex;
		curGroup.bone_dgs[boneIndex].oai   = object.index;
		curGroup.bone_dgs[boneIndex].opi   = srcGroup.object[object.index].ppi;
		if(transparent) {
			curGroup.entity[index].sprite = &curGroup.spriteT[boneIndex];
			curGroup.bone_dgs[boneIndex].sai = curGroup.totalSpritesT++;

			for(si32 i = boneIndex + 1, j = 1; i < lastIndex; i++, j++) {
/*				csi32 entityIndex = index + j;

			entity[group][index].part[j] = &entity[group][entityIndex];

			entity[group][entityIndex].part[0]  = &entity[group][index];
			entity[group][entityIndex].bone     = &bone[group][i];
			entity[group][entityIndex].geometry = &bone_dgs[group][i];
			entity[group][entityIndex].sprite   = &spriteO[group][i];*/
				curGroup.bone_dgs[i].sai = curGroup.totalSpritesT++;
				curGroup.bone_dgs[i].oai = object.index;
				curGroup.bone_dgs[i].opi = boneIndex;
			}
		} else {
			curGroup.entity[index].sprite = &curGroup.spriteO[boneIndex];
			curGroup.bone_dgs[boneIndex].sai = curGroup.totalSpritesO++;

			for(si32 i = boneIndex + 1, j = 1; i < lastIndex; i++, j++) {
/*				csi32 entityIndex = index + j;

			entity[group][index].part[j] = &entity[group][entityIndex];

			entity[group][entityIndex].part[0]  = &entity[group][index];
			entity[group][entityIndex].bone     = &bone[group][i];
			entity[group][entityIndex].geometry = &bone_dgs[group][i];
			entity[group][entityIndex].sprite   = &spriteO[group][i];*/
				curGroup.bone_dgs[i].sai = curGroup.totalSpritesO++;
				curGroup.bone_dgs[i].oai = object.index;
				curGroup.bone_dgs[i].opi = boneIndex;
			}
		}

		return { index, group };
	}

	cID64 CreateEntity(cui32 group, ID32 object, cVEC3Df position, cVEC3Df orientation, cVEC3Df size) {
		cui32 index = entGroup[group].totalEntities++;

		if(index >= MAX_ENTITIES) return { 0x080000001, 0x0 };

		const OBJECT_GROUP &srcGroup = objGroup[object.group];
				ENTITY_GROUP &curGroup = entGroup[group];

		csi8 partCount = srcGroup.object[object.index].qc;

		csi32 boneIndex = curGroup.totalBones;
		csi32 lastIndex = (curGroup.totalBones += partCount + 1);

		curGroup.totalBone_DGS = curGroup.totalBones;

		curGroup.entity[index].part[0]     = &curGroup.entity[index];
		curGroup.entity[index].bone        = &curGroup.bone[boneIndex];
		curGroup.entity[index].geometry    = &curGroup.bone_dgs[boneIndex];
		curGroup.entity[index].sprite      = &curGroup.spriteO[boneIndex];
		curGroup.entity[index].brain       = { 1, 25, 1, 191, 191, 239, -1 };
		curGroup.entity[index].vbd         = { 1.0f, 1.0f, 1.0f };
		curGroup.entity[index].vbt         = 0;
		curGroup.entity[index].numParts    = partCount;
		curGroup.entity[index].soundIndex  = -1;
		curGroup.entity[index].objectIndex = object.index;
		curGroup.entity[index].boneIndex   = boneIndex;
		curGroup.bone[boneIndex].mass    = 1.0f;
		curGroup.bone[boneIndex].tension = 1.0f;
		curGroup.bone[boneIndex].strInt  = 100.0f;
		curGroup.bone[boneIndex].curTemp = 293.0f;
		curGroup.bone[boneIndex].lsTemp  = 273.0f;
		curGroup.bone[boneIndex].usTemp  = 373.0f;
		curGroup.bone[boneIndex].sac     = 1.0f;
		curGroup.bone[boneIndex].cbd     = { 0.875f, 0.875f, 0.875f };
		curGroup.bone[boneIndex].cbt     = 3;
		curGroup.bone_dgs[boneIndex].pos  = position;
		curGroup.bone_dgs[boneIndex].rot  = orientation;
		curGroup.bone_dgs[boneIndex].sai  = curGroup.totalSpritesO++;
		curGroup.bone_dgs[boneIndex].size = size;
		curGroup.bone_dgs[boneIndex].opi  = srcGroup.object[object.index].ppi;
		curGroup.bone_dgs[boneIndex].aft  = 1.0f;
		curGroup.bone_dgs[boneIndex].afc  = 0;
		curGroup.bone_dgs[boneIndex].afo  = 0;
		curGroup.bone_dgs[boneIndex].oai  = object.index;
		curGroup.bone_dgs[boneIndex].pbi  = boneIndex;
		curGroup.bone_dgs[boneIndex].obi  = boneIndex;
		curGroup.spriteO[boneIndex].pmc = Fix8x4({ 1.0f, 1.0f, 1.0f, 1.0f });
		curGroup.spriteO[boneIndex].gtc = Fix8x4({ 1.0f, 1.0f, 1.0f, 1.0f });
		curGroup.spriteO[boneIndex].gev = Fix16(0.0f,-127.0f, 255.0f);
		curGroup.spriteO[boneIndex].ems = Fix16(1.0f, 514.0f);
		curGroup.spriteO[boneIndex].nms = Fix8(1.0f);
		curGroup.spriteO[boneIndex].rms = Fix8(1.0f);
		curGroup.spriteO[boneIndex].pms = Fix8(1.0f);

		for(si32 i = boneIndex + 1; i < lastIndex; i++) {
/*			csi32 entityIndex = index + j;

			entity[group][index].part[j] = &entity[group][entityIndex];

			entity[group][entityIndex].part[0]   = &entity[group][index];
			entity[group][entityIndex].bone      = &curGroup.bone[i];
			entity[group][entityIndex].geometry  = &curGroup.bone_dgs[i];
			entity[group][entityIndex].sprite    = &curGroup.spriteO[i];
			entity[group][entityIndex].brain.tb  = -1;
			entity[group][entityIndex].soundBank = -1;
*/
			curGroup.bone[i] = { { 0.0f, 0.0f, 0.0f }, 1.0f, 1.0f, 100.0f, 293.0f, 273.0f, 373.0f, 1.0f, { 0.875f, 0.875f, 0.875f }, 3 };
			curGroup.bone_dgs[i].size = { 1.0f, 1.0f, 1.0f };
			curGroup.bone_dgs[i].sai  = curGroup.totalSpritesO++;
			curGroup.bone_dgs[i].opi  = srcGroup.object[object.index].ppi + i - boneIndex;
			curGroup.bone_dgs[i].aft  = 1.0f;
			curGroup.bone_dgs[i].afc  = 0;
			curGroup.bone_dgs[i].afo  = 0;
			curGroup.bone_dgs[i].oai  = object.index;
			curGroup.bone_dgs[i].pbi  = boneIndex;
			curGroup.bone_dgs[i].obi  = boneIndex;
			curGroup.spriteO[i] = { Fix8x4({ 1.0f, 1.0f, 1.0f, 1.0f }), Fix8x4({ 1.0f, 1.0f, 1.0f, 1.0f }), Fix16(0.0f,-127.0f, 255.0f), Fix16(1.0f, 514.0f),
											Fix8(1.0f), Fix8(1.0f), Fix8(1.0f) };
		}

		return { index, group };
	}

	cui32 CreateBone(cui32 entityGroup, cVEC3Df position, cVEC3Df orientation, cVEC3Df size, cbool transparent) {
		cui32 boneIndex = entGroup[entityGroup].totalBones++;

		if(boneIndex >= MAX_BONES) return 0x080000001;

		cENTITY_GROUP &curGroup = entGroup[entityGroup];

		curGroup.bone[boneIndex].mass    = 1.0f;
		curGroup.bone[boneIndex].tension = 1.0f;
		curGroup.bone[boneIndex].strInt  = 100.0f;
		curGroup.bone[boneIndex].curTemp = 293.0f;
		curGroup.bone[boneIndex].lsTemp  = 273.0f;
		curGroup.bone[boneIndex].usTemp  = 373.0f;
		curGroup.bone[boneIndex].sac     = 1.0f;
		curGroup.bone[boneIndex].cbd     = { 0.875f, 0.875f, 0.875f };
		curGroup.bone[boneIndex].cbt     = 3;
		curGroup.bone_dgs[boneIndex].pos  = position;
		curGroup.bone_dgs[boneIndex].rot  = orientation;
		curGroup.bone_dgs[boneIndex].size = size;
		curGroup.bone_dgs[boneIndex].aft  = 1.0f;
		curGroup.bone_dgs[boneIndex].afc  = 0;
		curGroup.bone_dgs[boneIndex].afo  = 0;
		if(transparent) {
			curGroup.bone_dgs[boneIndex].sai = entGroup[entityGroup].totalSpritesT++;
			curGroup.spriteT[boneIndex] = { Fix8x4({ 1.0f, 1.0f, 1.0f, 1.0f }), Fix8x4({ 1.0f, 1.0f, 1.0f, 1.0f }),
													  Fix16(0.0f, -127.0f, 255.0f), Fix16(1.0f, 514.0f), Fix8(1.0f), Fix8(1.0f), Fix8(1.0f) };
		} else {
			curGroup.bone_dgs[boneIndex].sai = entGroup[entityGroup].totalSpritesO++;
			curGroup.spriteO[boneIndex] = { Fix8x4({ 1.0f, 1.0f, 1.0f, 1.0f }), Fix8x4({ 1.0f, 1.0f, 1.0f, 1.0f }),
													  Fix16(0.0f, -127.0f, 255.0f), Fix16(1.0f, 514.0f), Fix8(1.0f), Fix8(1.0f), Fix8(1.0f) };
		}

		return boneIndex;
	}

	inline void SetPart(cID32 object, csi8 part, cVEC3Df position, cVEC3Df orientation, cVEC2Df size, cVEC6Df recoilState, cVEC4Df texCoords, cui8 atlasIndex) const {
///--- To do...
	}

	inline void SetPart(csi16 objectGroup, csi32 partIndex, cVEC3Df position, cVEC3Df orientation, cVEC2Df size, cVEC6Df recoilState, cVEC4Df texCoords, cui8 atlasIndex) const {
		objGroup[objectGroup].part[partIndex].pos     = position;
		objGroup[objectGroup].part[partIndex].rot     = orientation;
		objGroup[objectGroup].part[partIndex].size    = size;
		objGroup[objectGroup].part[partIndex].trans   = { recoilState.pos, NULL, recoilState.ori };
		objGroup[objectGroup].part[partIndex].tc      = Fix16x4(texCoords, 32768.0f);
		objGroup[objectGroup].part[partIndex].ai_bits = atlasIndex;
	}

	inline void SetBrain(csi16 entityGroup, csi32 entityIndex, cui8 thoughtLatency, cui8 thoughtsPerSecond, cui8 maxSimulThoughts, cui8 thoughtDisruption, cui8 decisionDisruption, cui8 decisionConfidence, csi16 bankIndex) const {
		entGroup[entityGroup].entity[entityIndex].brain = { thoughtLatency, thoughtsPerSecond, maxSimulThoughts, thoughtDisruption,
																			 decisionDisruption, decisionConfidence, bankIndex };
	}

	inline void SetSound(csi16 entityGroup, csi32 entityIndex, csi16 soundBankIndex) const {
		entGroup[entityGroup].entity[entityIndex].soundIndex = soundBankIndex;
	}

	inline void SetBits(csi16 entityGroup, csi32 entityIndex, cui64 bits) const {
		entGroup[entityGroup].entity[entityIndex].bits = bits;
	}

	inline void SetBone(csi16 entityGroup, csi32 boneIndex, cVEC3Df velocity, cfl32 mass, cfl32 tension, cfl32 structIntegrity, cfl32 safeAcidStrength, cui16 bits) const {
		entGroup[entityGroup].bone[boneIndex].vel     = velocity;
		entGroup[entityGroup].bone[boneIndex].mass    = mass;
		entGroup[entityGroup].bone[boneIndex].tension = tension;
		entGroup[entityGroup].bone[boneIndex].strInt  = structIntegrity;
		entGroup[entityGroup].bone[boneIndex].sac     = safeAcidStrength;
		entGroup[entityGroup].bone[boneIndex].bits    = bits;
	}

	inline void SetCollision(csi16 entityGroup, csi32 boneIndex, cVEC3Df boundingSize, cui8 boundingType) const {
		entGroup[entityGroup].bone[boneIndex].cbd = boundingSize;
		entGroup[entityGroup].bone[boneIndex].cbt = boundingType;
	}

	inline void SetTemps(csi16 entityGroup, csi32 boneIndex, cfl32 current, cfl32 lowestSafe, cfl32 highestSafe) const {
		entGroup[entityGroup].bone[boneIndex].curTemp = current;
		entGroup[entityGroup].bone[boneIndex].lsTemp  = lowestSafe;
		entGroup[entityGroup].bone[boneIndex].usTemp  = highestSafe;
	}

	inline void SetFuel(csi16 entityGroup, csi32 boneIndex, cui8 fuelIndex, cui8 fuelType, cui32 fuelLevel, cui16 fuelRate) const {
		entGroup[entityGroup].bone[boneIndex].fct[fuelIndex] = fuelType;
		entGroup[entityGroup].bone[boneIndex].fl[fuelIndex]  = fuelLevel;
		entGroup[entityGroup].bone[boneIndex].fcr[fuelIndex] = fuelRate;
	}

	inline void SetBoneGS(csi16 entityGroup, csi32 boneIndex, cVEC3Df position, cVEC3Df orientation, cVEC3Df size, cfl32 lerp, cfl32 animFrameTime, cui8 animFrameCount, cui8 animFrameOS) const {
		entGroup[entityGroup].bone_dgs[boneIndex].pos  = position;
		entGroup[entityGroup].bone_dgs[boneIndex].lerp = lerp;
		entGroup[entityGroup].bone_dgs[boneIndex].rot  = orientation;
		entGroup[entityGroup].bone_dgs[boneIndex].aft  = animFrameTime;
		entGroup[entityGroup].bone_dgs[boneIndex].afc  = animFrameCount;
		entGroup[entityGroup].bone_dgs[boneIndex].afo  = animFrameOS;
		entGroup[entityGroup].bone_dgs[boneIndex].size = size;
	}

	inline void SetBonePS(csi16 entityGroup, csi32 boneIndex, cVEC4Df tintColour, cVEC4Df paintColour, cfl32 paintMap, cfl32 globalEmission, cfl32 emissionMap, cfl32 normalMap, cfl32 roughnessMap) const {
		entGroup[entityGroup].spriteO[boneIndex] = { Fix8x4(paintColour), Fix8x4(tintColour), Fix16(globalEmission, -128.0f, 128.0f),
																	Fix16(emissionMap, 0.0f, 128.0f), Fix8(normalMap), Fix8(roughnessMap), Fix8(paintMap) };
	}

	inline si32 Cull(ui32aptrc arrayVisible, ui32ptrc arrayUnchanged, csi32 entityGroup, csi8 threadCount) {
		static ui8 uiTHREADS = 0;

		if(!arrayVisible) { MAPMAN_THREAD_STATUS.m128i_u8[0] &= 0x0F3; return 0; }

		cui8 threadBits = ENTMAN_THREAD_STATUS.m128i_u8[0];

		if(!(threadBits & 0x01)) threadData[0] = { &entGroup[entityGroup], arrayVisible };
		if(!(threadBits & 0x02)) threadData[1] = { &entGroup[entityGroup], arrayUnchanged };

		///- Stall/skip? if status if 'busy'
		while(ENTMAN_THREAD_STATUS.m128i_u8[0] & 0x03) Sleep(1);
		//if(bits) return si32(0x080000000 | bits);
		//ENTMAN_THREAD_STATUS.m128i_u8[0] &= 0x0FC;

		switch(threadCount) {
//		case 0;
//			Cull_Nonvisible_and_Unchanged(&threadData[0]);
//			return 3;
		case 1:
			ENTMAN_THREAD_STATUS.m128i_u8[0] |= 0x03;
			if(!(uiTHREADS & 0x03)) {
				HANDLE thread0 = (HANDLE)_beginthread(_ET_Cull_Nonvisible_and_Unchanged, 0, &threadData[0]);
				SetThreadIdealProcessor(thread0, 4);
				SetThreadPriority(thread0, -1);
				uiTHREADS |= 0x03;
			}
			return 3;
		case 2:
			ENTMAN_THREAD_STATUS.m128i_u8[0] |= 0x03;
			if(!(uiTHREADS & 0x03)) {
				//HANDLE thread0 = (HANDLE)_beginthread(_ET_Cull_Nonvisible_Simple, 0, &threadData[0]);
				HANDLE thread0 = (HANDLE)_beginthread(_ET_Cull_Nonvisible_Accurate, 0, &threadData[0]);
				SetThreadIdealProcessor(thread0, 4);
				SetThreadPriority(thread0, -1);
				HANDLE thread1 = (HANDLE)_beginthread(_ET_Cull_Unchanged, 0, &threadData[1]);
				SetThreadIdealProcessor(thread1, 5);
				SetThreadPriority(thread1, -1);
				uiTHREADS |= 0x03;
			}
			return 3;
		case -1:
			ENTMAN_THREAD_STATUS.m128i_u8[0] |= 0x01;
			if(!(uiTHREADS & 0x01)) {
				//HANDLE thread0 = (HANDLE)_beginthread(_ET_Cull_Nonvisible_Simple, 0, &threadData[0]);
				HANDLE thread0 = (HANDLE)_beginthread(_ET_Cull_Nonvisible_Accurate, 0, &threadData[0]);
				SetThreadPriority(thread0, -1);
				uiTHREADS |= 0x01;
			}
			return 1;
		case -2:
			ENTMAN_THREAD_STATUS.m128i_u8[0] |= 0x02;
			if(!(uiTHREADS & 0x02)) {
				HANDLE thread1 = (HANDLE)_beginthread(_ET_Cull_Unchanged, 0, &threadData[1]);
				SetThreadPriority(thread1, -1);
				uiTHREADS |= 0x02;
			}
			return 2;
		}

		ENTMAN_THREAD_STATUS.m128i_u8[0] &= 0x0F3;

		return 0;
	}

	inline cVEC4Du32 WaitForCulling(cDWORD sleepDelay) const {
		while(ENTMAN_THREAD_STATUS.m128i_u8[0] & 0x03) Sleep(sleepDelay);

		cVEC2Du64 entManThreadData = (cVEC2Du64 &)ENTMAN_THREAD_STATUS;

		ENTMAN_THREAD_STATUS.m128i_u64[0] &= 0x0C;
		ENTMAN_THREAD_STATUS.m128i_u64[1] = 0;

		return { ui32(entManThreadData.x >> 4) & 0x03FFFFFFF, ui32(entManThreadData.x >> 34) & 0x03FFFFFFF,
					ui32(entManThreadData.y) & 0x03FFFFFFF, ui32(entManThreadData.y >> 30) & 0x03FFFFFFF };
	}
};

static void _ET_Cull_Nonvisible_Accurate(ptr) {}
static void _ET_Cull_Unchanged(ptr) {}

static void _ET_Cull_Nonvisible_and_Unchanged(ptr threadData) {
		static si64 frequencyTics, startTics, endTics;
		QueryPerformanceFrequency((LARGE_INTEGER *)&frequencyTics);

	CLASS_CAMERAS       * const camMan = (CLASS_CAMERAS *)ptrLib[1];
	CLASS_ENTITYMANAGER * const entMan = (CLASS_ENTITYMANAGER *)ptrLib[3];

	cEMTDptr      data[2] = { (cEMTDptr)threadData, (cEMTDptr)threadData + 1 };
	ENTITY_GROUP &group   = *(*data[0]).group;

	cui32 entityCount = group.totalEntities;

	ui32ptrc nearBones = (*data[0]).entityVis[0];
	ui32ptrc medBones  = (*data[0]).entityVis[1];
	ui32ptrc farBones  = (*data[0]).entityVis[2];
	ui32ptrc modBones  = (*data[1]).entityMod;

	SSE4Df32 sphereData[8];

	ui64 modCount, nearCount, medCount, farCount;
	ui32 i, j, k, l;

	ENTMAN_THREAD_STATUS.m128i_u8[0] |= 0x0C;

	do {
		while(!(ENTMAN_THREAD_STATUS.m128i_u8[0] & 0x03)) Sleep(1);

		QueryPerformanceCounter((LARGE_INTEGER *)&startTics);

		nearCount = medCount = farCount = 0;
		modCount = 0;

		// Test for modified entities
		for(i = 0; i < entityCount; i++) {
			cui64 bitOS   = (ui64)0x01 << (i & 0x03F);
			cui32 qwordOS = i >> 6;

			if(group.entityMod[qwordOS] & bitOS) {
				for(j = 0; j <= group.entity[i].numParts; j++)
					modBones[modCount++] = group.entity[i].boneIndex + j;
				group.entityMod[qwordOS] ^= bitOS;
			}
		}
		for(; i & 0x01F; i++)
			modBones[modCount++] = 0x0FFFFFFFF;

		// Calculate each entity's distance-to-camera and reject out-of-view entities
		// Sort nearest-to-furthest into L.O.D. lists for input assembler
		for(i = 0; i < entityCount; i += j) {
			for(j = 0; j < 8; j++) {
				csi32 entityIndex = i + j;

				((VEC3Df &)sphereData[j]) = group.entity[entityIndex].geometry->pos;
///--- Switch according to bounding type
				sphereData[j].vector.w = group.entity[entityIndex].vbd.x;
			}

			cui8 visible = camMan->SphereFrustumIntersect8(sphereData, 0);

			for(k = 0; k < j; k++)
				if(visible & (0x01 << k)) {
					csi32 entityIndex = i + k;
//					cui32 QWordOS = index >> 6;
//					cui64 bitOS   = (ui64)0x01 << (index & 0x03F);
					cENTITY &curEntity = group.entity[entityIndex];

					cfl32 distance = camMan->DistanceFromCamera(sphereData[k].xmm, 0);
					// Change hard limits to LOD scalars
					if(curEntity.geometry->size.x > 0.0f) {
//								if(distance < 128.0f)
						for(l = 0; l <= curEntity.numParts; l++)
							nearBones[nearCount++] = curEntity.boneIndex + l;
//								else if(distance < 512.0f)
//									medBones[medCount++] = index;
//								else if(distance < 2048.0f)
//									farBones[farCount++] = index;
					}
				}
		}

		ENTMAN_THREAD_STATUS.m128i_u64[0] = (medCount << 34) | (nearCount << 4) | 0x0C;
		ENTMAN_THREAD_STATUS.m128i_u64[1] = (modCount << 30) | farCount;

			QueryPerformanceCounter((LARGE_INTEGER *)&endTics);
			sysData.culling.entity.time   = double(endTics - startTics) / double(frequencyTics) * 1000.0;
			sysData.culling.entity.mod    = (ui32)modCount;
			sysData.culling.entity.vis[0] = (ui32)nearCount;
			sysData.culling.entity.vis[1] = (ui32)medCount;
			sysData.culling.entity.vis[2] = (ui32)farCount;
	} while(ENTMAN_THREAD_STATUS.m128i_u64[0] & 0x0C);
}
