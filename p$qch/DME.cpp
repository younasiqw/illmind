#include "DME.h"

namespace DME {
	DrawModelExecute_t g_fnOriginalDrawModelExecute;
	void __fastcall Hooked_DrawModelExecute(void* thisptr, void* edx, SSDK::IMatRenderContext* ctx, const SSDK::DrawModelState_t& state, const SSDK::ModelRenderInfo_t& pInfo, SSDK::matrix3x4_t* pCustomBoneToWorld) {
		
		g_fnOriginalDrawModelExecute(thisptr, ctx, state, pInfo, pCustomBoneToWorld);
	}
}