#include "InterpFix.h"

#include "OptionsManager.h"

namespace InterpFix {
	IsPlayingTimeDemo_t g_fnOriginalPlayingTimeDemo;
	bool __fastcall HookedIsPlayingTimeDemo(void* thisptr, void* edx) {
		bool bResult = g_fnOriginalPlayingTimeDemo(thisptr);
		if (OptionsManager.bInterpLagComp) {
			CBaseEntity *pLocalPlayer = CBaseEntity::GetLocalPlayer();

			if (SSDK::I::Engine()->isInGame() && SSDK::I::Engine()->isConnected())
				if (pLocalPlayer->IsAlive() && !pLocalPlayer->InBuyZone())
					return true;
		}
		return bResult;
	}
}