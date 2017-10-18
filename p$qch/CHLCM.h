#pragma once

#include "SqchSDK\Interfaces.h"
#include "EntityStructs.h"
#include "OptionsManager.h"
#include "Utilities.h"
#include "CAutoWall.h"
#include "CFixMove.h"
#include "CPredictionSystem.h"

namespace CHLCM {
	typedef void(__thiscall* CreateMoveCHL_t)(void*, int, float, bool);

	extern CreateMoveCHL_t g_fnOriginalCreateMoveCHL;

	void __stdcall Hooked_CreateMove_CHL(int sequence_number, float input_sample_frametime, bool active, bool& sendpacket);
}