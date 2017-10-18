 #include "OView.h"

namespace OView {
	OverrideView_t g_fnOriginalOverrideView;
	void __fastcall Hooked_OverrideView(void* thisptr, int edx, SSDK::CViewSetup* pSetup) {
		CBaseEntity *pLocalPlayer = CBaseEntity::GetLocalPlayer();
		if (pLocalPlayer->isValidPlayer(true)) {
			if (!pLocalPlayer->isScoped())
				pSetup->fov += 20;
		}

		g_fnOriginalOverrideView(thisptr, pSetup);
	}
}