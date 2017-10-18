#pragma once
#include "SqchSDK\Interfaces.h"
#include "SqchSDK\Offsets.h"

#include "EntityStructs.h"

class CPredictionSystem {
public:
	CPredictionSystem();
	void StartPrediction(SSDK::CUserCmd *cmd);
	void EndPrediction();

private:
	CBaseEntity *pLocalPlayer;
	float _flOldCurTime;
	float _flOldFrameTime;
	int* m_nPredictionRandomSeed;
	CBaseEntity* m_pPredictionPlayer;

	//pPrediction
	SSDK::Vector vecBaseVelocity;
	float flFallVelocity;
};