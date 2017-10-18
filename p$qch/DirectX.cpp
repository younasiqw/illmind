#include "DirectX.h"
#include <fstream>

/*void PlayerListRenderTab()
{
	for (int i = 1; i < SSDK::I::Engine()->GetMaxClients(); ++i) {
		CBaseEntity *pClient = (CBaseEntity*)SSDK::I::EntityList()->GetClientEntity(i);
		if (pClient) {
			if (*pClient->GetTeamNum() != *CBaseEntity::GetLocalPlayer()->GetTeamNum()) {
				SSDK::PlayerInfo entInfo;
				SSDK::I::Engine()->GetPlayerInfo(i, &entInfo);
				if (ImGui::CollapsingHeader(entInfo.szName)) {
					ImGui::Checkbox(XorStr("Use custom settings"), &OptionsManager.aCacheAngle[i].bCustomSetts);
					ImGui::Checkbox(XorStr("Resolve angle"), &OptionsManager.aCacheAngle[i].bCorrect);
					static const char *szResolveType[] = { XorStr("Simple"), XorStr("Delta calc"), XorStr("Delta calc 2"), XorStr("Anti-Anti-Resolver"), XorStr("Spread Perfect"), XorStr("NoSpread") };
					ImGui::Combo(XorStr("Resolver type"), &OptionsManager.aCacheAngle[i].iResolverType, szResolveType, ARRAYSIZE(szResolveType));
					ImGui::Checkbox(XorStr("Awall Prioritize visible parts"), &OptionsManager.aCacheAngle[i].bPrioritizeVis);
					ImGui::Checkbox(XorStr("Prioritize selected hitbox"), &OptionsManager.aCacheAngle[i].bPrioritize);
					ImGui::SliderInt(XorStr("Select hitbox"), &OptionsManager.aCacheAngle[i].iHitbox, 0, 20);
					ImGui::SliderFloat(XorStr("Min dmg"), &OptionsManager.aCacheAngle[i].flMinDmg, 0, 100);
					ImGui::Checkbox(XorStr("Auto baim"), &OptionsManager.aCacheAngle[i].bAutoBaim);
					ImGui::SliderInt(XorStr("Baim after shot"), &OptionsManager.aCacheAngle[i].iAutoBaimAferShot, 0, 10);
					ImGui::SliderInt(XorStr("Select hitbox auto baim"), &OptionsManager.aCacheAngle[i].iHitboxAutoBaim, 0, 20);
					ImGui::Checkbox(XorStr("Disable interp/Lag compensate"), &OptionsManager.aCacheAngle[i].bInterpLagComp);
				}
			}
		}
	}
}*/

void HvHRenderTab()
{
	ImGui::Checkbox(XorStr("Fakelag"), &OptionsManager.bFakeLag);

	ImGui::SliderInt(XorStr("Choke Packets"), &OptionsManager.iFakeLagAmount, 0, 14);

	ImGui::Spacing();

	ImGui::Checkbox(XorStr("AA Enable"), &OptionsManager.bAA);

	ImGui::Text("Anti-Aim: ");
	ImGui::BeginChild("#aa", ImVec2(0, 200), true);
	static const char *szAAPitch[] = { XorStr("Down"), XorStr("Up"), XorStr("Zero"), XorStr("Fake Zero") };
	ImGui::Combo(XorStr("Pitch"), &OptionsManager.iAAPitch, szAAPitch, ARRAYSIZE(szAAPitch));

	static const char *szRealAAYaw[] = { XorStr("Lower Body Yaw"), XorStr("Fast Spin"), XorStr("Lower Body Yaw Breaker") };
	ImGui::Combo(XorStr("Real Yaw"), &OptionsManager.iAARealYaw, szRealAAYaw, ARRAYSIZE(szRealAAYaw));

	ImGui::SliderInt(XorStr("Real Yaw Add"), &OptionsManager.iAARealAdd, 0, 180);

	static const char *szFakeAAYaw[] = { XorStr("Lower Body Yaw"), XorStr("Fast Spin"), XorStr("Flip Jitter"), XorStr("Wide Flip Jitter") };
	ImGui::Combo(XorStr("Fake Yaw"), &OptionsManager.iAAFakeYaw, szFakeAAYaw, ARRAYSIZE(szFakeAAYaw));

	ImGui::SliderInt(XorStr("Fake Yaw Add"), &OptionsManager.iAAFakeAdd, 0, 180);

	ImGui::EndChild();

	ImGui::Spacing();

	ImGui::Checkbox("Fake Walk (Shift)", &OptionsManager.bSlowMo);
}

void AimbotRenderTab()
{
	ImGui::Checkbox(XorStr("Auto Shoot"), &OptionsManager.bAutoShoot);
	ImGui::Checkbox(XorStr("Auto Pistol"), &OptionsManager.bAutoPistol);
	ImGui::Checkbox(XorStr("Auto Scope"), &OptionsManager.bAutoScope);
	ImGui::Checkbox(XorStr("Silent aim"), &OptionsManager.bSilentAim);

	ImGui::Spacing();

	ImGui::SliderInt(XorStr("HitChance"), &OptionsManager.iHitchance, 0, 100);
	ImGui::SliderInt(XorStr("HitChance Max Seeds"), &OptionsManager.iHitChanceSeed, 0, 256);
	ImGui::Checkbox(XorStr("Resolver"), &OptionsManager.bCorrect);
	ImGui::Checkbox(XorStr("Bruteforce (ALT)"), &OptionsManager.bBruteforceResolver);

	ImGui::Spacing();

	ImGui::Checkbox(XorStr("Calculate Best Point"), &OptionsManager.bPrioritizeVis);
	ImGui::Checkbox(XorStr("Prioritize Selected Hitbox"), &OptionsManager.bPrioritize);
	ImGui::SliderInt(XorStr("Select Hitbox"), &OptionsManager.iHitbox, 0, 20);
	ImGui::SliderFloat(XorStr("Min Damage"), &OptionsManager.flMinDmg, 0, 100, "%.2f");
	ImGui::SliderFloat(XorStr("Point Scale"), &OptionsManager.flPointScale, 0, 1, "%.2f");

	ImGui::Spacing();

	ImGui::Checkbox(XorStr("Auto Body-Aim"), &OptionsManager.bAutoBaim);
	ImGui::SliderInt(XorStr("Body-Aim After Shots"), &OptionsManager.iAutoBaimAferShot, 0, 10);
	ImGui::SliderInt(XorStr("Select Hitbox Auto Body-Aim"), &OptionsManager.iHitboxAutoBaim, 0, 20);

	ImGui::Spacing();

	ImGui::Checkbox(XorStr("Disable Interp/Lag Compensate"), &OptionsManager.bInterpLagComp);
}

void MiscRenderTab()
{
	static char clantag[127] = "";
	static char name[127] = "";
	static char clearclantag[127] = "";
	ImGui::Checkbox(XorStr("No Flash"), &OptionsManager.bNoFlash);
	ImGui::InputText("##CLANTAG", clantag, 127);
	ImGui::SameLine();
	if (ImGui::Button("Set Clantag", ImVec2(-1, 0)))
		Utilities::SetClanTag(clantag, clantag);
	if (ImGui::Button("Clear Clantag", ImVec2(-1, 0)))
		Utilities::SetClanTag(clearclantag, clearclantag);
	ImGui::Checkbox(XorStr("Animated Clantag"), &OptionsManager.bClantagchanger);
	ImGui::InputText("##NAME", name, 127);
	ImGui::SameLine();
	if (ImGui::Button("Set Name", ImVec2(-1, 0)))
	{
		static SSDK::ConVar *hahaname = SSDK::I::CVar()->FindVar(XorStr("name"));
		hahaname->m_fnChangeCallbacks.m_Size = 0;
		hahaname->SetValue(name);
	}
}

void VisualsRenderTab()
{
	ImGui::Checkbox(XorStr("Enable ESP"), &OptionsManager.bEspEnabled);
	if (OptionsManager.bEspEnabled) {
		ImGui::Checkbox(XorStr("Box"), &OptionsManager.bBoxESP);
		ImGui::Checkbox(XorStr("Name"), &OptionsManager.bPlayerName);

		ImGui::Spacing();

		ImGui::Checkbox(XorStr("Chams"), &OptionsManager.bChams);
		ImGui::Checkbox(XorStr("Bones"), &OptionsManager.bBones);
	}

	ImGui::Spacing();

	ImGui::Checkbox(XorStr("No Visual Recoil"), &OptionsManager.bNoVisRecoil);
	ImGui::Checkbox(XorStr("No Scope"), &OptionsManager.bNoScope);
	ImGui::SliderInt(XorStr("View Model Offset"), &OptionsManager.iFov, 68, 130);

	ImGui::Spacing();

	ImGui::Checkbox("Third Person (P)", &OptionsManager.iThirdPerson);
//TODO:	//ImGui::Checkbox("Night Mode", &OptionsManager.bNightMode);

//TODO: //ImGui::Checkbox("Indicators", &OptionsManager.bIndicators);
//TODO:	//ImGui::Checkbox("Bullet Tracers", &OptionsManager.bBulletTracers);
//TODO:	//ImGui::Checkbox("Draw Anti-Aim Lines", &OptionsManager.bAngle);
}



namespace DirectX {
	EndScene_t g_fnOriginalEndScene;
	Reset_t g_fnOriginalReset;

	WNDPROC g_pOldWindowProc;
	HWND g_hWindow;

	bool g_IsInitialized = false;

	HRESULT __stdcall Hooked_EndScene(IDirect3DDevice9* pDevice) {
		if (!g_IsInitialized) {
			ImGui_ImplDX9_Init(g_hWindow, pDevice);

			ImGuiStyle * style = &ImGui::GetStyle();

			style->WindowPadding = ImVec2(8, 8);
			style->WindowRounding = 0.0f;
			style->FramePadding = ImVec2(4, 3);
			style->FrameRounding = 0.0f;
			style->ItemSpacing = ImVec2(12, 8);
			style->AntiAliasedLines = true;
			style->AntiAliasedShapes = true;
			style->Alpha = 1.0f;
			style->CurveTessellationTol = 1.25f;
			style->ItemInnerSpacing = ImVec2(8, 6);
			style->DisplayWindowPadding = ImVec2(22, 22);
			style->IndentSpacing = 21.0f;
			style->ScrollbarSize = 15.0f;
			style->ScrollbarRounding = 9.0f;
			style->GrabMinSize = 5.0f;
			style->GrabRounding = 3.0f;

			style->Colors[ImGuiCol_Text] = ImVec4(0.80f, 0.80f, 0.83f, 1.00f);
			style->Colors[ImGuiCol_TextDisabled] = ImVec4(0.24f, 0.23f, 0.29f, 1.00f);
			style->Colors[ImGuiCol_WindowBg] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
			style->Colors[ImGuiCol_ChildWindowBg] = ImVec4(0.07f, 0.07f, 0.09f, 1.00f);
			style->Colors[ImGuiCol_PopupBg] = ImVec4(0.07f, 0.07f, 0.09f, 1.00f);
			style->Colors[ImGuiCol_Border] = ImVec4(0.80f, 0.80f, 0.83f, 0.88f);
			style->Colors[ImGuiCol_BorderShadow] = ImVec4(0.92f, 0.91f, 0.88f, 0.00f);
			style->Colors[ImGuiCol_FrameBg] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
			style->Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.24f, 0.23f, 0.29f, 1.00f);
			style->Colors[ImGuiCol_FrameBgActive] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
			style->Colors[ImGuiCol_TitleBg] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
			style->Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(1.00f, 0.98f, 0.95f, 0.75f);
			style->Colors[ImGuiCol_TitleBgActive] = ImVec4(0.07f, 0.07f, 0.09f, 1.00f);
			style->Colors[ImGuiCol_MenuBarBg] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
			style->Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
			style->Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.80f, 0.80f, 0.83f, 0.31f);
			style->Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
			style->Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
			style->Colors[ImGuiCol_ComboBg] = ImVec4(0.19f, 0.18f, 0.21f, 1.00f);
			style->Colors[ImGuiCol_CheckMark] = ImVec4(0.80f, 0.80f, 0.83f, 0.31f);
			style->Colors[ImGuiCol_SliderGrab] = ImVec4(0.80f, 0.80f, 0.83f, 0.31f);
			style->Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
			style->Colors[ImGuiCol_Button] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
			style->Colors[ImGuiCol_ButtonHovered] = ImVec4(0.24f, 0.23f, 0.29f, 1.00f);
			style->Colors[ImGuiCol_ButtonActive] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
			style->Colors[ImGuiCol_Header] = ImVec4(0.12f, 0.09f, 0.12f, 1.00f);
			style->Colors[ImGuiCol_HeaderHovered] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
			style->Colors[ImGuiCol_HeaderActive] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
			style->Colors[ImGuiCol_Column] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
			style->Colors[ImGuiCol_ColumnHovered] = ImVec4(0.24f, 0.23f, 0.29f, 1.00f);
			style->Colors[ImGuiCol_ColumnActive] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
			style->Colors[ImGuiCol_ResizeGrip] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
			style->Colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
			style->Colors[ImGuiCol_ResizeGripActive] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
			style->Colors[ImGuiCol_CloseButton] = ImVec4(0.40f, 0.39f, 0.38f, 0.16f);
			style->Colors[ImGuiCol_CloseButtonHovered] = ImVec4(0.40f, 0.39f, 0.38f, 0.39f);
			style->Colors[ImGuiCol_CloseButtonActive] = ImVec4(0.40f, 0.39f, 0.38f, 1.00f);
			style->Colors[ImGuiCol_PlotLines] = ImVec4(0.40f, 0.39f, 0.38f, 0.63f);
			style->Colors[ImGuiCol_PlotLinesHovered] = ImVec4(0.25f, 1.00f, 0.00f, 1.00f);
			style->Colors[ImGuiCol_PlotHistogram] = ImVec4(0.40f, 0.39f, 0.38f, 0.63f);
			style->Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(0.25f, 1.00f, 0.00f, 1.00f);
			style->Colors[ImGuiCol_TextSelectedBg] = ImVec4(0.25f, 1.00f, 0.00f, 0.43f);
			style->Colors[ImGuiCol_ModalWindowDarkening] = ImVec4(1.00f, 0.98f, 0.95f, 0.73f);

			g_IsInitialized = true;
		}
		else {
			ImGui::GetIO().MouseDrawCursor = OptionsManager.bShowMenu;

			//Begins a new ImGui frame.
			ImGui_ImplDX9_NewFrame();



			ImGui::SetNextWindowPos(ImVec2(100, 200), ImGuiSetCond_FirstUseEver);

			// Testing Not 100% Done Badly Coded Will Recode
			if (OptionsManager.bShowMenu)
			{
#define IM_ARRAYSIZE(_ARR)  ((int)(sizeof(_ARR)/sizeof(*_ARR)))
				ImGui::SetNextWindowSize(ImVec2(960, 645));
				if (ImGui::Begin("                                                       Illmind.tk >< Illminded", &OptionsManager.bShowMenu, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar))
				{
					const char* tabs[] = {
						"Aimbot",
						"HvH",
						"Misc",
						"Visuals",
					};
					static int page = 0;
					for (int i = 0; i < IM_ARRAYSIZE(tabs); i++)
					{
						int distance = i == page ? 0 : i > page ? i - page : page - i;

						if (ImGui::Button(tabs[i], ImVec2(ImGui::GetWindowSize().x / IM_ARRAYSIZE(tabs) - 9, 0)))
							page = i;

						if (i < IM_ARRAYSIZE(tabs) - 1)
							ImGui::SameLine();
					}
					ImGui::Separator();

					switch (page)
					{
					case 0:
						AimbotRenderTab();
						break;
					case 1:
						HvHRenderTab();
						break;
					case 2:
						MiscRenderTab();
						break;
					case 3:
						VisualsRenderTab();
						break;
					}

					ImGui::End();
				}
			}
			ImGui::Render();
		}

		return g_fnOriginalEndScene(pDevice);
	}

	HRESULT __stdcall Hooked_Reset(IDirect3DDevice9* pDevice, D3DPRESENT_PARAMETERS* pPresentationParameters) {
		if (!g_IsInitialized) return g_fnOriginalReset(pDevice, pPresentationParameters);
		ImGui_ImplDX9_InvalidateDeviceObjects();

		auto hr = g_fnOriginalReset(pDevice, pPresentationParameters);
		ImGui_ImplDX9_CreateDeviceObjects();
		return hr;
	}

	LRESULT __stdcall Hooked_WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
		switch (uMsg) {
		case WM_LBUTTONDOWN:
			OptionsManager.vecPressedKeys[VK_LBUTTON] = true;
			break;
		case WM_LBUTTONUP:
			OptionsManager.vecPressedKeys[VK_LBUTTON] = false;
			break;
		case WM_RBUTTONDOWN:
			OptionsManager.vecPressedKeys[VK_RBUTTON] = true;
			break;
		case WM_RBUTTONUP:
			OptionsManager.vecPressedKeys[VK_RBUTTON] = false;
			break;
		case WM_KEYDOWN:
			OptionsManager.vecPressedKeys[wParam] = true;
			break;
		case WM_KEYUP:
			OptionsManager.vecPressedKeys[wParam] = false;
			break;
		default: break;
		}

		static SSDK::ConVar *conMouseEnable = SSDK::I::CVar()->FindVar(XorStr("cl_mouseenable"));
		static bool isDown = false;
		static bool isClicked = false;
		if (OptionsManager.vecPressedKeys[VK_INSERT]) {
			isClicked = false;
			isDown = true;
		}
		else if (!OptionsManager.vecPressedKeys[VK_INSERT] && isDown) {
			isClicked = true;
			isDown = false;
		}
		else {
			isClicked = false;
			isDown = false;
		}

		if (isClicked) {
			if (OptionsManager.bShowMenu) {
				//CONFIG SAVER - LOAD
				std::ofstream ofs("ShemaleWare.save", std::ios::binary);
				ofs.write((char*)&OptionsManager, sizeof(OptionsManager));
			}
			OptionsManager.bShowMenu = !OptionsManager.bShowMenu;
			conMouseEnable->SetValue(!OptionsManager.bShowMenu);
		}


		if (g_IsInitialized && OptionsManager.bShowMenu && ImGui_ImplDX9_WndProcHandler(hWnd, uMsg, wParam, lParam))
			return true;

		return CallWindowProc(g_pOldWindowProc, hWnd, uMsg, wParam, lParam);
	}
}