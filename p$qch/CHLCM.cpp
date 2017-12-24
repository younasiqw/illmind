#include "CHLCM.h"

#include "CLagCompensation.h"
#include <time.h>
#include "DrawManager.h"
#include "Globals.h"

SSDK::QAngle CreateMoveAngles = SSDK::QAngle(0, 0, 0);

#define RANDOM_FLOAT( min, max ) (( min + 1 ) + ( ( ( float ) rand( ) ) / ( float ) RAND_MAX ) * ( max - ( min + 1 ) ))

namespace CHLCM {
	static CFixMove *FixMoveManager = new CFixMove();
	static CPredictionSystem *Pred = new CPredictionSystem();
	static DrawManager *DrawMgr = new DrawManager();
	static SSDK::ICvar *iCvar = new SSDK::ICvar();

	static float timer;
	static int iShotFired;
	static bool lby_updated_tick;
	static bool ToggleSide = false;

	float GetTraceFractionWorldProps(SSDK::Vector startpos, SSDK::Vector endpos) {
		SSDK::Ray_t ray;
		SSDK::Trace_t tr;
		SSDK::CTraceFilterWorldOnly filter;

		ray.Init(startpos, endpos);

		SSDK::I::EngineTrace()->TraceRay(ray, MASK_SHOT, &filter, &tr);

		return tr.fraction;
	}

	CreateMoveCHL_t g_fnOriginalCreateMoveCHL;
	void __stdcall Hooked_CreateMove_CHL(int sequence_number, float input_sample_frametime, bool active, bool& bSendPacket) {
		g_fnOriginalCreateMoveCHL((void*)SSDK::I::Client(), sequence_number, input_sample_frametime, active);

		if (SSDK::I::Engine()->isInGame() && SSDK::I::Engine()->isConnected()) {
			//VARS
			CBaseEntity *pLocalPlayer = CBaseEntity::GetLocalPlayer();

			if (!pLocalPlayer && !pLocalPlayer->IsAlive())
				return;

			//CHECK USERCMD
			SSDK::CVerifiedUserCmd* pVerif = &(*(SSDK::CVerifiedUserCmd**)((DWORD)SSDK::I::Input() + 0xF0))[sequence_number % 150];
			SSDK::CUserCmd *pCmd = &(*(SSDK::CUserCmd**)((DWORD)SSDK::I::Input() + 0xEC))[sequence_number % 150];

			if (pCmd && pVerif) {
				if (GetAsyncKeyState(0x4E) & 0x8000) {
					pCmd->tick_count = INT_MAX;
				}

				if (OptionsManager.bAutoHop) {
					pCmd->buttons |= IN_JUMP;

					if (pCmd->buttons & IN_JUMP && !(*pLocalPlayer->GetFlags() & (int)SSDK::EntityFlags::FL_ONGROUND)) {
						pCmd->buttons &= ~IN_JUMP;

						if (pCmd->mousedx > 1 || pCmd->mousedx < -1) {
							pCmd->sidemove = pCmd->mousedx < 0.f ? -450.f : 450.f;
						}
						else {
							pCmd->forwardmove = (10000.f) / (pLocalPlayer->GetVelocity()->Length2D() + 1);
							pCmd->sidemove = (pCmd->command_number % 2) == 0 ? -450.f : 450.f;
						}
					}
				}
				else {
					if (pCmd->buttons & IN_JUMP && !(*pLocalPlayer->GetFlags() & (int)SSDK::EntityFlags::FL_ONGROUND)) {
						pCmd->buttons &= ~IN_JUMP;

						if (pCmd->mousedx > 1 || pCmd->mousedx < -1) {
							pCmd->sidemove = pCmd->mousedx < 0.f ? -450.f : 450.f;
						}
						else {
							pCmd->forwardmove = (10000.f) / (pLocalPlayer->GetVelocity()->Length2D() + 1);
							pCmd->sidemove = (pCmd->command_number % 2) == 0 ? -450.f : 450.f;
						}
					}
				}

				//START PREDICTION
				Pred->StartPrediction(pCmd);
				FixMoveManager->Start(pCmd);

				//AIMBOT ROUTINE
				float flCurrentTime = SSDK::I::GlobalVars()->curtime;
				CBaseCombatWeapon *activeWeapon = pLocalPlayer->GetActiveWeapon();

				if (OptionsManager.bAutoPistol) // Badly Coded Auto Pistol will make new later.
				{
					if (pLocalPlayer->IsAlive() && pCmd->buttons & IN_ATTACK && activeWeapon->isPistol())
					{
						static bool FlipPistol = false;
						if (FlipPistol)
							pCmd->buttons &= ~IN_ATTACK;
						FlipPistol = !FlipPistol;
					}
				}

				if (OptionsManager.bAutoRevolver)
				{
					if (pLocalPlayer->IsAlive())
					{
						if (*activeWeapon->GetItemDefinitionIndex() == SSDK::EItemDefinitionIndex::weapon_revolver)
						{
							float flPostponeFireReady = activeWeapon->PostponeFireReady();
							pCmd->buttons |= IN_ATTACK;
							if (flPostponeFireReady > 0 && flPostponeFireReady <= (*pLocalPlayer->GetTickBase() * SSDK::I::GlobalVars()->interval_per_tick))
								pCmd->buttons &= ~IN_ATTACK;
						}
					}
				}


				for (int i = 1; i < SSDK::I::Engine()->GetMaxClients(); ++i) {
					CBaseEntity *pClient = (CBaseEntity*)SSDK::I::EntityList()->GetClientEntity(i);

					//Aimbot->Run(pCmd, bSendPacket, pClient);

					if (pClient->isValidPlayer()) {
						if (*pLocalPlayer->GetTeamNum() != *pClient->GetTeamNum()) {

							bool bPrioritize = OptionsManager.bPrioritize;
							int iHitbox = OptionsManager.iHitbox;
							bool bAutoBaim = OptionsManager.bAutoBaim;
							int iAutoBaimAferShot = OptionsManager.iAutoBaimAferShot;
							int iHitboxAutoBaim = OptionsManager.iHitboxAutoBaim;
							float flMinDmg = OptionsManager.flMinDmg;
							bool bInterpLagComp = OptionsManager.bInterpLagComp;

							if (OptionsManager.aCacheAngle[i].bCustomSetts) {
								bPrioritize = OptionsManager.aCacheAngle[i].bPrioritize;
								iHitbox = OptionsManager.aCacheAngle[i].iHitbox;
								bAutoBaim = OptionsManager.aCacheAngle[i].bAutoBaim;
								iAutoBaimAferShot = OptionsManager.aCacheAngle[i].iAutoBaimAferShot;
								iHitboxAutoBaim = OptionsManager.aCacheAngle[i].iHitboxAutoBaim;
								flMinDmg = OptionsManager.aCacheAngle[i].flMinDmg;
								bInterpLagComp = OptionsManager.aCacheAngle[i].bInterpLagComp;
							}

							if (bInterpLagComp)
								StartLagCompensation(pClient, pCmd);

							//AIMBOT
							SSDK::Vector vecClientTarget;

							if (Globals::ShotsFired > iAutoBaimAferShot && bAutoBaim)
								vecClientTarget = CAutowall::CalculateBestPoint(pClient, iHitboxAutoBaim, flMinDmg, bPrioritize);
							else
								vecClientTarget = CAutowall::CalculateBestPoint(pClient, iHitbox, flMinDmg, bPrioritize);

							if (vecClientTarget.IsValid()) {
								SSDK::QAngle qaNewViewAngle = Utilities::CalcAngle(pLocalPlayer->GetEyePos(), vecClientTarget) - *pLocalPlayer->AimPunch() * 2.f;

								if (Utilities::Clamp(qaNewViewAngle)) {
									pCmd->viewangles = qaNewViewAngle;

									//DEBUG
									//SSDK::I::Engine()->SetViewAngles(pCmd->viewangles);
									if (activeWeapon && activeWeapon->isValidWeapon()) {
										if (activeWeapon->NextPrimaryAttack() <= flCurrentTime) {
											if (!pLocalPlayer->isScoped() && activeWeapon->isWeaponScope()) {
												if (OptionsManager.bAutoScope)
													pCmd->buttons |= IN_ATTACK2;
											}
											else if (Utilities::HitChance(pLocalPlayer, pClient, activeWeapon, qaNewViewAngle, pCmd->random_seed, OptionsManager.iHitchance)) {
												if (OptionsManager.bAutoShoot) {
													if (activeWeapon->GetCurrentAmmo())
													{
														pCmd->buttons |= IN_ATTACK;
														Globals::FiredThatTick = true;
														Globals::ShotsFiredEnt[pClient->EntIndex()]++;
													}
												}
											}
										}
									}
								}
							}

						}
					}
				}

				//Calculate shot fired
				if (activeWeapon) {
					if (!(pCmd->buttons & IN_ATTACK) && activeWeapon->NextPrimaryAttack() <= flCurrentTime) {
						Globals::ShotsFired = 0;
						Globals::FiredThatTick = false;
					}
					else {
						Globals::ShotsFired += pLocalPlayer->GetShotFired();
					}
				}

				SSDK::QAngle Oldview = pCmd->viewangles;

				if (OptionsManager.bSilentAim) {
					if (pCmd->buttons & IN_ATTACK) {
						bSendPacket = false;
					}
					else {
						bSendPacket = true;
						pCmd->viewangles = FixMoveManager->GetOldAngle();
					}
				}

				//END PREDICTION
				FixMoveManager->Stop(pCmd);
				Pred->EndPrediction();

				//FAKE LAG FILS DE
				static int iTick;
				if (OptionsManager.bFakeLag) {
					OptionsManager.iFakeLagAmount = Utilities::FakelagCompensationBreak();

				}
				if (iTick < OptionsManager.iFakeLagAmount && !(pCmd->buttons & IN_ATTACK)) {
					bSendPacket = false;
					iTick++;
				}
				else {
					bSendPacket = true;
					iTick = 0;
				}

				SSDK::QAngle cache = pCmd->viewangles;

				if (OptionsManager.bSlowMo) {
					CBaseEntity *pLocal = CBaseEntity::GetLocalPlayer();
					if (GetAsyncKeyState(VK_SHIFT))
					{
						static int iChoked = -1;
						iChoked++;
						if (iChoked < 1)
						{
							bSendPacket = false;

							pCmd->tick_count += 10;
							pCmd->command_number += 7 + pCmd->tick_count % 2 ? 0 : 1;

							pCmd->buttons |= pCmd->upmove == IN_BACK;
							pCmd->forwardmove = pCmd->sidemove = 0.f;
						}
						else
						{
							bSendPacket = true;
							iChoked = -1;

							SSDK::I::GlobalVars()->frametime *= pLocalPlayer->GetVelocity()->Length2D() / 1.f;
							pCmd->buttons |= pCmd->forwardmove == IN_FORWARD;
						}
					}
				}

				/* ~~ Messy anti aim ~~ */
				if (OptionsManager.bAA) {
					if (activeWeapon) {
						if (!(pCmd->buttons & IN_USE) &&
							!(pCmd->buttons & IN_ATTACK) ||
							(activeWeapon->isGrenade())) {
							if (!(activeWeapon->getGrenade()->GetThrowTime() > 0.f)) {

								float flCurrentTime = SSDK::I::GlobalVars()->curtime;

								/* ~~ DO NOT USE BELOW vvv COMPLETE SHIT I JUST DID IT FOR FUN AND THE ANTIAIM GOT PEOPLE TO SHOOT FAKES ~~ */

								lby_updated_tick = false;
								float latency = SSDK::I::Engine()->GetNetChannelInfo()->GetLatency(SSDK::I::Engine()->FLOW_OUTGOING);
								static float flNextTimeUpdateLBY = 0; // fl cur time + 1.1f;
								if (pLocalPlayer->GetVelocity()->Length() == 0)
								{
									if (flNextTimeUpdateLBY + latency < flCurrentTime || flNextTimeUpdateLBY - flCurrentTime > 1.32f)
									{
										lby_updated_tick = true;
										bSendPacket = false;

										flNextTimeUpdateLBY = flCurrentTime + 1.1f;
									}
								}

								/* ~~ DO NOT USE ABOVE ^^^ COMPLETE SHIT I JUST DID IT FOR FUN AND THE ANTIAIM GOT PEOPLE TO SHOOT FAKES ~~ */

								FixMoveManager->Start(pCmd);
								SSDK::QAngle newAngle = pCmd->viewangles;

								float yaw;
								static int state = 0;
								static bool flip = false;

								static float flTimeUpdate = 1.1f;
								static float flNextTimeUpdate = flCurrentTime + flTimeUpdate;
								if (flCurrentTime >= flNextTimeUpdate) {
									flip = !flip;
									state = 0;
								}

								if (flNextTimeUpdate < flCurrentTime || flNextTimeUpdate - flCurrentTime > 10.f)
									flNextTimeUpdate = flCurrentTime + flTimeUpdate;

								// Start of Yaws

								// Start of Real yaw

								if (!bSendPacket)
								{
									// Lower Body Yaw
									if (OptionsManager.iAARealYaw == 0)
										yaw = lby_updated_tick ? 180 : -180;

									// Fast Spin
									else if (OptionsManager.iAARealYaw == 1)
										yaw = lby_updated_tick ? fmod((*pLocalPlayer->GetTickBase() * SSDK::I::GlobalVars()->interval_per_tick) / .1f * 360.f, 360.f) - 180 : fmod((*pLocalPlayer->GetTickBase() * SSDK::I::GlobalVars()->interval_per_tick) / .1f * 360.f, 360.f) + 180;

									// Lower Body Yaw Breaker (b1g meme)
									else if (OptionsManager.iAARealYaw == 2)
									{
										static bool bJitter; bJitter = !bJitter;
										static bool quick_switch;
										static int state;

										if (lby_updated_tick)
										{
											if (state < 2)
											{
												state++;
												quick_switch = true;
											}
											if (state >= 2)
											{
												state = 0;
												quick_switch = false;
											}
										}
										else
											quick_switch = false;

										yaw = quick_switch ? -180 : (180 + bJitter ? 30 : -30);
									}

									if (ToggleSide)
										yaw += OptionsManager.iAARealAdd + 180;
									else
										yaw += OptionsManager.iAARealAdd;
								}
								// End of Real yaw

								// Start of Fake yaw
								if (bSendPacket)
								{
									// Lower Body Yaw
									if (OptionsManager.iAAFakeYaw == 0)
										yaw = lby_updated_tick ? -180 : 180;

									// Fast Spin
									else if (OptionsManager.iAAFakeYaw == 1)
										yaw = lby_updated_tick ? fmod(((int)pLocalPlayer->GetTickBase() * SSDK::I::GlobalVars()->interval_per_tick) / .1f * 360.f, 360.f) + 180 : fmod(((int)pLocalPlayer->GetTickBase() * SSDK::I::GlobalVars()->interval_per_tick) / .1f * 360.f, 360.f) - 180;

									// Flip Jitter (kidua)
									else if (OptionsManager.iAAFakeYaw == 2)
									{
										if (flip) {
											yaw = 90.f + state * 34.f;
											if (state > 1)
												yaw = 90.f + 34.8f;
											if (state < 3)
												state++;
										}
										else {
											yaw = -90.f - state * 34.f;
											if (state > 1)
												yaw = -90.f - 34.8f;
											if (state < 3)
												state++;
										}
									}

									// Wide Flip Jitter (kidua)
									else if (OptionsManager.iAAFakeYaw == 3)
									{
										if (flip) {
											yaw = 110.f + state * 34.f;
											if (state < 3)
												state++;
										}
										else {
											yaw = -110.f - state * 34.f;
											if (state < 3)
												state++;
										}
									}

									if (ToggleSide)
										yaw += OptionsManager.iAAFakeAdd + 180;
									else
										yaw += OptionsManager.iAAFakeAdd;
								}
								// End of Fake yaw

								if (yaw)
									newAngle.y += yaw;

								// End of Yaws


								// Start of Pitch

								if (OptionsManager.iAAPitch == 0)
									newAngle.x = 89.f;
								else if (OptionsManager.iAAPitch == 1)
									newAngle.x = -89.f;
								else if (OptionsManager.iAAPitch == 2)
									newAngle.x = 0.f;
								else if (OptionsManager.iAAPitch == 3)
									newAngle.x = 1080.f;

								// End of Pitch

								if (Utilities::Clamp(newAngle)) {
									pCmd->viewangles = newAngle;
								}

								FixMoveManager->Stop(pCmd);
							}
						}
					}
				}

				/* ~~ Solid memes ;p ~~ */

				static float pressTime;
				if (GetAsyncKeyState(VK_DOWN) && abs(pressTime - SSDK::I::GlobalVars()->curtime) > 0.5)
				{
					pressTime = SSDK::I::GlobalVars()->curtime;
					ToggleSide = !ToggleSide;
				}

				pVerif->m_cmd = *pCmd;
				pVerif->m_crc = pCmd->GetChecksum();
			}
			if (!bSendPacket && OptionsManager.iAAFakeYaw >= 0)
				CreateMoveAngles = pCmd->viewangles;
		}
	}
}

#undef RANDOM_FLOAT
