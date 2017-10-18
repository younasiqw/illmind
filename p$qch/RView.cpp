#include "RView.h"

namespace RView {
	RenderView_t g_fnOriginalRenderView;
	void __fastcall Hooked_RenderView(void* thisptr, void* edx, SSDK::CViewSetup &view, SSDK::CViewSetup &hudViewSetup, int nClearFlags, int whatToDraw){
			CBaseEntity *pLocalPlayer = CBaseEntity::GetLocalPlayer();
			if (pLocalPlayer->isValidPlayer(true)) {
				if (!pLocalPlayer->isScoped())
					view.fov += OptionsManager.iFov;
			}

		g_fnOriginalRenderView(thisptr, view, hudViewSetup, nClearFlags, whatToDraw);
	}
}