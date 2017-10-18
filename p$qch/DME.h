#pragma once

#include <fstream>

#include "SqchSDK\Interfaces.h"
#include "EntityStructs.h"

namespace DME {
	typedef void(__thiscall* DrawModelExecute_t)(void*, SSDK::IMatRenderContext*, const SSDK::DrawModelState_t&, const SSDK::ModelRenderInfo_t&, SSDK::matrix3x4_t*);

	extern DrawModelExecute_t g_fnOriginalDrawModelExecute;

	//Funcs
	void __fastcall Hooked_DrawModelExecute(void* thisptr, void* edx, SSDK::IMatRenderContext* ctx, const SSDK::DrawModelState_t& state, const SSDK::ModelRenderInfo_t& pInfo, SSDK::matrix3x4_t* pCustomBoneToWorld);
}