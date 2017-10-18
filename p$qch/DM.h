#pragma once

#include <fstream>

#include "SqchSDK\Interfaces.h"
#include "EntityStructs.h"
#include "OptionsManager.h"
#include "CAutoWall.h"

namespace DM {
	typedef void(__thiscall* DrawModel_t)(void*, SSDK::DrawModelResults_t *pResults, const SSDK::DrawModelInfo_t& info,
		SSDK::matrix3x4_t *pBoneToWorld, float *pFlexWeights, float *pFlexDelayedWeights, const SSDK::Vector &modelOrigin, int flags);

	extern DrawModel_t g_fnOriginalDrawModel;

	void __fastcall Hooked_DrawModel(void* thisptr, void* edx, SSDK::DrawModelResults_t *pResults, const SSDK::DrawModelInfo_t& info,
		SSDK::matrix3x4_t *pBoneToWorld, float *pFlexWeights, float *pFlexDelayedWeights, const SSDK::Vector &modelOrigin, int flags = 0);

	void InitChams();
}