#include "Hooks.h"

#include "SqchSDK\Interfaces.h"
#include "CHurtListener.h"
#include "CRSListener.h"
#include "CBulletListener.h"
#include "BeginFrame.h"

namespace Hooks {
	std::unique_ptr<HookTable::VFTableHook> g_pClientHook = nullptr;
	std::unique_ptr<HookTable::VFTableHook> g_pStudioRenderHook = nullptr;
	std::unique_ptr<HookTable::VFTableHook> g_pModelRenderHook = nullptr;
	std::unique_ptr<HookTable::VFTableHook> g_pVGuiHook = nullptr;
	std::unique_ptr<HookTable::VFTableHook> g_pClientModeHook = nullptr;
	std::unique_ptr<HookTable::VFTableHook> g_pD3DDevice9Hook = nullptr;
	std::unique_ptr<HookTable::VFTableHook> g_pEngineHook = nullptr;
	std::unique_ptr<HookTable::VFTableHook> g_pViewRenderHook = nullptr;
	std::unique_ptr<HookTable::VFTableHook> g_pFireBulletHook = nullptr;

	HookTable::VFTableHook *g_pEngineTraceHook = nullptr;

	void __declspec(naked) __stdcall HookedCreateMoveNaked(int sequence_number, float input_sample_frametime, bool active)
	{
		__asm
		{
			PUSH EBX				// push sendpacket from ebx
			PUSH ESP				// push address of sendpackt
			PUSH[ESP + 0x14]		// push active
			PUSH[ESP + 0x14]		// push input_sample_frametime
			PUSH[ESP + 0x14]		// push sequence_number
			CALL CHLCM::Hooked_CreateMove_CHL	// call our own CreateMove function
			POP EBX					// pop sendpacket to ebx
	  		RET 0x0C				// remove 3 agruments from stack and return
		}
	}

	typedef void(__thiscall* ClipRay)(void*, const SSDK::Ray_t &ray, unsigned int fMask, SSDK::IHandleEntity *pEnt, SSDK::CGameTrace *pTrace);
	ClipRay g_fnOriginalClipRay;
	void __fastcall Hooked_ClipRay(void* thisptr, void* edx, const SSDK::Ray_t &ray, unsigned int fMask, SSDK::IHandleEntity *pEnt, SSDK::CGameTrace *pTrace) {
		g_fnOriginalClipRay(thisptr, ray, fMask, pEnt, pTrace);
		pTrace->fraction = 1.f;
	}

	typedef void(__thiscall* TraceRay)(void*, const SSDK::Ray_t &ray, unsigned int fMask, SSDK::ITraceFilter *pTraceFilter, SSDK::CGameTrace *pTrace);
	TraceRay g_fnOriginalTraceRay;
	void __fastcall Hooked_TracRay(void* thisptr, void* edx, const SSDK::Ray_t &ray, unsigned int fMask, SSDK::ITraceFilter *pTraceFilter, SSDK::CGameTrace *pTrace) {
		/*SSDK::Ray_t ray2;
		SSDK::Vector vec = CBaseEntity::GetLocalPlayer()->GetEyePos();
		vec.z += 20.f;
		SSDK::Vector vec2 = CBaseEntity::GetLocalPlayer()->GetEyePos();
		vec.z += 25.f;
		ray2.Init(vec, vec2);*/
		g_fnOriginalTraceRay(thisptr, ray, fMask, pTraceFilter, pTrace);
		pTrace->fraction = 1.f;
	}

	typedef void(__thiscall* RayCollide)(void*, const SSDK::Ray_t &ray, unsigned int fMask, SSDK::ICollideable *pCollide, SSDK::CGameTrace *pTrace);
	RayCollide g_fnOriginalRayCollide;
	void __fastcall Hooked_RayCollide(void* thisptr, void* edx, const SSDK::Ray_t &ray, unsigned int fMask, SSDK::ICollideable *pCollide, SSDK::CGameTrace *pTrace) {
		g_fnOriginalRayCollide(thisptr, ray, fMask, pCollide, pTrace);
		pTrace->fraction = 1.f;
	}

	void Initialize() {
		//-----------------------------------------------------------------------------------------------

		NetvarManager::Instance()->CreateDatabase();
		
		DrawManager::InitializeFont(XorStr("Verdana"), 15);

		DM::InitChams();

		//-----------------------------------------------------------------------------------------------

		//SSDK::I::Engine()->ExecuteClientCmd(XorStr("clear"));
		//SSDK::I::CVar()->ConsoleColorPrintf(SSDK::Color(255, 0, 255), XorStr("\ShemaleWare, insert to open menu\n"));

		//-----------------------------------------------------------------------------------------------

		auto dwDevice = **(uint32_t**)(SSDK::O::FindSignature(XorStr("shaderapidx9.dll"), XorStr("A1 ? ? ? ? 50 8B 08 FF 51 0C")) + 1);
		//auto oTE_FireBullets = *reinterpret_cast<SSDK::C_TEFireBullets**>(SSDK::O::FindSignature("client.dll", "8B D1 B8 ? ? ? ? 51") + 0x91);

		//-----------------------------------------------------------------------------------------------

		g_pClientHook = std::make_unique<HookTable::VFTableHook>((HookTable::PPDWORD)SSDK::I::Client(), true);
		g_pStudioRenderHook = std::make_unique<HookTable::VFTableHook>((HookTable::PPDWORD)SSDK::I::StudioRender(), true);
		g_pModelRenderHook = std::make_unique<HookTable::VFTableHook>((HookTable::PPDWORD)SSDK::I::ModelRender(), true);
		g_pVGuiHook = std::make_unique<HookTable::VFTableHook>((HookTable::PPDWORD)SSDK::I::VGUIPanel(), true);
		g_pClientModeHook = std::make_unique<HookTable::VFTableHook>((HookTable::PPDWORD)SSDK::I::ClientMode(), true);
		g_pD3DDevice9Hook = std::make_unique<HookTable::VFTableHook>((HookTable::PPDWORD)dwDevice, true);
		g_pEngineHook = std::make_unique<HookTable::VFTableHook>((HookTable::PPDWORD)SSDK::I::Engine(), true);
		//g_pViewRenderHook = std::make_unique<HookTable::VFTableHook>((HookTable::PPDWORD)SSDK::I::ViewRender(), true);
		//g_pFireBulletHook = std::make_unique<HookTable::VFTableHook>((HookTable::PPDWORD)oTE_FireBullets, true);

		while (!(DirectX::g_hWindow = FindWindowA(XorStr("Valve001"), NULL))) Sleep(200);

		if (DirectX::g_hWindow)
			DirectX::g_pOldWindowProc = (WNDPROC)SetWindowLongPtr(DirectX::g_hWindow, GWLP_WNDPROC, (LONG_PTR)DirectX::Hooked_WndProc);

		//Drawing hooks
		PT::g_fnOriginalPaintTraverse = g_pVGuiHook->Hook(41, (PT::PaintTraverse_t)PT::Hooked_PaintTraverse);
		DirectX::g_fnOriginalReset = g_pD3DDevice9Hook->Hook(16, DirectX::Hooked_Reset);
		DirectX::g_fnOriginalEndScene = g_pD3DDevice9Hook->Hook(42, DirectX::Hooked_EndScene);
		DM::g_fnOriginalDrawModel = g_pStudioRenderHook->Hook(29, (DM::DrawModel_t)DM::Hooked_DrawModel);
		//DME::g_fnOriginalDrawModelExecute = g_pModelRenderHook->Hook(21, (DME::DrawModelExecute_t)DME::Hooked_DrawModelExecute);

		//Update hooks
		FSN::g_fnOriginalFrameStageNotify = g_pClientHook->Hook(36, (FSN::FrameStageNotify_t)FSN::Hooked_FrameStageNotify);

		//Move hooks
		CHLCM::g_fnOriginalCreateMoveCHL = g_pClientHook->Hook(21, (CHLCM::CreateMoveCHL_t)HookedCreateMoveNaked);

		//RenderView hook -> Can't hook this, it crashes even if correct meh
		//RView::g_fnOriginalRenderView = g_pViewRenderHook->Hook(6, (RView::RenderView_t)RView::Hooked_RenderView);

		//OverrideView hooks
		OView::g_fnOriginalOverrideView = g_pClientModeHook->Hook(18, (OView::OverrideView_t)OView::Hooked_OverrideView);

		//Interpolate Fix Hook
		InterpFix::g_fnOriginalPlayingTimeDemo = g_pEngineHook->Hook(84, (InterpFix::IsPlayingTimeDemo_t)InterpFix::HookedIsPlayingTimeDemo);

		// Begin frame machine working/broke cbf checking lol
	// 	BeginFrame::g_fnOriginalBeginFrame = g_pStudioRenderHook->Hook(9, (BeginFrame::BeginFrame_t)BeginFrame::Hooked_BeginFrame);

		// Fire Bullet Hook
		//TEFireBullet::g_fnOriginalFireBullets = g_pFireBulletHook->Hook(7, (TEFireBullet::FireBullets_t)TEFireBullet::HookedTEFireBulletsPostDataUpdate);

		/*g_pEngineTraceHook = new HookTable::VFTableHook((HookTable::PPDWORD)SSDK::I::EngineTrace(), true);
		g_fnOriginalClipRay = g_pEngineTraceHook->Hook(3, (ClipRay)Hooked_ClipRay);
		g_fnOriginalRayCollide = g_pEngineTraceHook->Hook(4, (RayCollide)Hooked_RayCollide);
		g_fnOriginalTraceRay = g_pEngineTraceHook->Hook(5, (TraceRay)Hooked_TracRay);*/

		//-----------------------------------------------------------------------------------------------

		//ProxyHook
		/*SSDK::ClientClass *pClass = SSDK::I::Client()->GetAllClasses();
		while (pClass) {
			const char *pszName = pClass->m_pRecvTable->GetName();
			if (!strcmp(pszName, XorStr("DT_CSPlayer"))) {
				for (int i = 0; i < pClass->m_pRecvTable->m_nProps; ++i) {
					SSDK::RecvProp *pProp = pClass->m_pRecvTable->GetTableProp(i);
					const char *name = pProp->GetName();
					if (!strcmp(name, XorStr("m_flLowerBodyYawTarget"))) {
						pProp->SetProxyFn(LowerYaw);
					}
				}
			}
			pClass = pClass->m_pNext;
		}*/

		//------------------------------------------------------------------------------------------------

		//Setup convar
		SSDK::ConVar *matprocess_enable = SSDK::I::CVar()->FindVar(XorStr("mat_postprocess_enable"));
		SSDK::SpoofedConvar *matprocess_enable_spoofed = new SSDK::SpoofedConvar(matprocess_enable);
		matprocess_enable_spoofed->SetInt(0);

		SSDK::ConVar *interpolate = SSDK::I::CVar()->FindVar(XorStr("cl_interpolate"));
		SSDK::SpoofedConvar *interpolate_spoofed = new SSDK::SpoofedConvar(interpolate);
		interpolate_spoofed->SetInt(0);

		//------------------------------------------------------------------------------------------------

		// REGISTER LISTENERS
		CHurtListener::singleton()->init();
		CRSListener::singleton()->init();
//TODO:	//	CBulletListener::singleton()->init();
		// LISTENERS REGISTERED

		//CONFIG SAVER - LOAD
		std::ifstream ifs("ShemaleWare.save", std::ios::binary);
		ifs.read((char*)&OptionsManager, sizeof(OptionsManager));
	}

	void RestoreAllTable() {
		g_pClientHook->RestoreTable();
		g_pModelRenderHook->RestoreTable();
		g_pVGuiHook->RestoreTable();
		g_pClientModeHook->RestoreTable();
		g_pD3DDevice9Hook->RestoreTable();
		g_pEngineHook->RestoreTable();

		SetWindowLongPtr(DirectX::g_hWindow, GWLP_WNDPROC, (LONG_PTR)DirectX::g_pOldWindowProc);
	}
}