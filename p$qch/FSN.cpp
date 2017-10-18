#include "FSN.h"
#include "Globals.h"


#define	HITGROUP_GENERIC	0
#define	HITGROUP_HEAD		1
#define	HITGROUP_CHEST		2
#define	HITGROUP_STOMACH	3
#define HITGROUP_LEFTARM	4	
#define HITGROUP_RIGHTARM	5
#define HITGROUP_LEFTLEG	6
#define HITGROUP_RIGHTLEG	7
#define HITGROUP_GEAR		10

#define TICK_INTERVAL			( SSDK::I::GlobalVars()->interval_per_tick )
#define TIME_TO_TICKS( dt )		( (int)( 0.5f + (float)(dt) / TICK_INTERVAL ) )

namespace FSN {
	FrameStageNotify_t g_fnOriginalFrameStageNotify;
	VOID __fastcall Hooked_FrameStageNotify(void* thisptr, void* edx, SSDK::ClientFrameStage_t curStage) {
		CBaseEntity *pLocalPlayer = CBaseEntity::GetLocalPlayer();

		SSDK::Vector vecAimPunch;
		SSDK::Vector vecViewPunch;

		SSDK::Vector* pAimPunch = nullptr;
		SSDK::Vector* pViewPunch = nullptr;

		static bool DoThirdperson = false;

		if (SSDK::I::Engine()->isInGame()) {
			if (curStage == SSDK::ClientFrameStage_t::FRAME_NET_UPDATE_POSTDATAUPDATE_START) {

				for (int i = 1; i < SSDK::I::Engine()->GetMaxClients(); ++i) {
					CBaseEntity *pClient = (CBaseEntity*)SSDK::I::EntityList()->GetClientEntity(i);
					if (pClient->isValidPlayer()) {

						// RESOLVER 
						bool bCorrect = OptionsManager.bCorrect;

						if (bCorrect)
						{
							/* ~~ Meme resolver as I deleted old one sorry <3 ~~ */
							pClient->GetEyeAngles()->y = *(float*)pClient->GetLowerBodyYaw();
						}
					}
				}
			}

			if (curStage == SSDK::ClientFrameStage_t::FRAME_RENDER_START) {
				if (pLocalPlayer && pLocalPlayer->IsAlive())
				{
					*(int*)((uintptr_t)pLocalPlayer + 0xA30) = SSDK::I::GlobalVars()->framecount; //we'll skip occlusion checks now
					*(int*)((uintptr_t)pLocalPlayer + 0xA28) = 0;//clear occlusion flags
				}


				static SSDK::QAngle vecAngles;
				SSDK::I::Engine()->GetViewAngles(vecAngles);

				if (OptionsManager.iThirdPerson && pLocalPlayer->IsAlive() && DoThirdperson)
				{
					if (!SSDK::I::Input()->m_fCameraInThirdPerson)
					{
						SSDK::I::Input()->m_fCameraInThirdPerson = true;
						SSDK::I::Input()->m_vecCameraOffset = SSDK::Vector(vecAngles.x, vecAngles.y, 141);
					}
				}
				else
				{
					SSDK::I::Input()->m_fCameraInThirdPerson = false;
					SSDK::I::Input()->m_vecCameraOffset = SSDK::Vector(vecAngles.x, vecAngles.y, 0);
				}

				/* 0x50 == P */

				static float pressTime;
				if (GetAsyncKeyState(0x50) && abs(pressTime - SSDK::I::GlobalVars()->curtime) > 0.5)
				{
					pressTime = SSDK::I::GlobalVars()->curtime;
					DoThirdperson = !DoThirdperson;
				}

				if (OptionsManager.bNoVisRecoil) {
					pAimPunch = pLocalPlayer->AimPunch();
					pViewPunch = pLocalPlayer->ViewPunch();

					vecAimPunch = *pAimPunch;
					vecViewPunch = *pViewPunch;

					*pAimPunch = SSDK::QAngle(0, 0, 0);
					*pViewPunch = SSDK::QAngle(0, 0, 0);
				}

				if (OptionsManager.bBruteforceResolver) {
					for (int i = 1; i < SSDK::I::Engine()->GetMaxClients(); ++i) {
						CBaseEntity *pClient = (CBaseEntity*)SSDK::I::EntityList()->GetClientEntity(i);
						if (pClient->isValidPlayer()) {
							if (GetAsyncKeyState(VK_MENU)) {
								if (!pClient->isMoving()) {
									pClient->GetEyeAngles()->y = *pClient->GetLowerBodyYaw();
									int angle = rand() % 5;
									switch (angle) {
									case 0:pClient->GetEyeAngles()->y += 135; break;
									case 2:pClient->GetEyeAngles()->y += 155; break;
									case 3:pClient->GetEyeAngles()->y += 165; break;
									case 4:pClient->GetEyeAngles()->y += 175; break;
									case 5:pClient->GetEyeAngles()->y += 185; break;
									}
								}
								else {
									pClient->GetEyeAngles()->y = *(float*)pClient->GetLowerBodyYaw();
									pClient->GetEyeAngles()->y += 180;
								}
							}
						}
					}
				}

				if (OptionsManager.bClantagchanger) {

					static std::string cur_clantag = "		Illmind";
					static int old_time;

					static int i = 0;

					if (SSDK::I::Engine()->isInGame()) {
						if (i > 32)
						{
							Utilities::marquee(cur_clantag);
							Utilities::SetClanTag(cur_clantag.c_str(), cur_clantag.c_str());
							i = 0;
						}
						else
						{
							i++;
						}
					}
				}


				if (OptionsManager.bNoFlash)
					if (*pLocalPlayer->flashDuration() > 0.f)
						*pLocalPlayer->flashDuration() = 0.f;

				// Display real angles on antiaim
				if (*(bool*)((DWORD)SSDK::I::Input() + 0xA5))
					*(SSDK::QAngle*)((DWORD)pLocalPlayer + 0x031C8) = SSDK::QAngle(pLocalPlayer->GetEyeAngles()->x, CreateMoveAngles.y, 0);//OptionsManager.qaLocalEyesAngle;
			}
		}

		if (curStage == SSDK::ClientFrameStage_t::FRAME_NET_UPDATE_END)
		{
			/* ~~ Stored Resolver Variables Here ~~ */
		}

		//Original
		g_fnOriginalFrameStageNotify(thisptr, curStage);

		if (OptionsManager.bNoVisRecoil) {
			if (curStage == SSDK::ClientFrameStage_t::FRAME_RENDER_START) {
				if (pAimPunch && pViewPunch) {
					*pAimPunch = vecAimPunch;
					*pViewPunch = vecViewPunch;
				}
			}
		}


		if (curStage == SSDK::ClientFrameStage_t::FRAME_RENDER_END) {
			//UpdateClientAnim
			for (int i = 1; i < SSDK::I::Engine()->GetMaxClients(); ++i) {
				CBaseEntity *pClient = (CBaseEntity*)SSDK::I::EntityList()->GetClientEntity(i);
				if (pClient->isValidPlayer()) {
					pClient->UpdateClientSideAnimation();
				}
			}
		}
	}
}