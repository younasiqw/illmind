#pragma once

#include "SqchSDK\Interfaces.h"

#include "EntityStructs.h"

#include "Utilities.h"

namespace RView {
	typedef void(__thiscall* RenderView_t)(void*, SSDK::CViewSetup &view, SSDK::CViewSetup &hudViewSetup, int nClearFlags, int whatToDraw);

	extern RenderView_t g_fnOriginalRenderView;

	void __fastcall Hooked_RenderView(void* thisptr, void* edx, SSDK::CViewSetup &view, SSDK::CViewSetup &hudViewSetup, int nClearFlags, int whatToDraw);
}