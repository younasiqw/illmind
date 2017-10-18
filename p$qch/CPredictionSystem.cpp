#include "CPredictionSystem.h"

CPredictionSystem::CPredictionSystem() {
	m_nPredictionRandomSeed = *reinterpret_cast<int**>(SSDK::O::FindSignature(XorStr("client.dll"), XorStr("A3 ? ? ? ? 66 0F 6E 86")) + 0x1);
	m_pPredictionPlayer = *reinterpret_cast<CBaseEntity**>(SSDK::O::FindSignature(XorStr("client.dll"), XorStr("89 35 ? ? ? ? F3 0F 10 48 20")) + 0x2);
}

void CPredictionSystem::StartPrediction(SSDK::CUserCmd* cmd) {
	CBaseEntity *pPlayer = CBaseEntity::GetLocalPlayer();
	//pPred
	vecBaseVelocity = *pPlayer->GetBaseVelocity();
	flFallVelocity = *pPlayer->GetFallVelocity();

	SSDK::O::MoveHelper()->SetHost(pPlayer);

	m_pPredictionPlayer = pPlayer;
	*m_nPredictionRandomSeed = cmd->random_seed;

	_flOldCurTime = SSDK::I::GlobalVars()->curtime;
	_flOldFrameTime = SSDK::I::GlobalVars()->frametime;

	SSDK::I::GlobalVars()->curtime = *pPlayer->GetTickBase() * SSDK::I::GlobalVars()->interval_per_tick;
	SSDK::I::GlobalVars()->frametime = SSDK::I::GlobalVars()->interval_per_tick;

	SSDK::I::GameMovement()->StartTrackPredictionErrors(pPlayer);

	SSDK::CMoveData moveData;
	memset(&moveData, 0, sizeof(moveData));
	SSDK::I::Prediction()->SetupMove(m_pPredictionPlayer, cmd, SSDK::O::MoveHelper(), &moveData);
	SSDK::I::GameMovement()->ProcessMovement(m_pPredictionPlayer, &moveData);
	SSDK::I::Prediction()->FinishMove(m_pPredictionPlayer, cmd, &moveData);
}

void CPredictionSystem::EndPrediction() {
	CBaseEntity *pPlayer = CBaseEntity::GetLocalPlayer();

	SSDK::I::GameMovement()->FinishTrackPredictionErrors(pPlayer);

	SSDK::I::GlobalVars()->curtime = _flOldCurTime;
	SSDK::I::GlobalVars()->frametime = _flOldFrameTime;
	*m_nPredictionRandomSeed = -1;
	SSDK::O::MoveHelper()->SetHost(nullptr);

	//pPred
	*pPlayer->GetBaseVelocity() = vecBaseVelocity;
	*pPlayer->GetFallVelocity() = flFallVelocity;
}