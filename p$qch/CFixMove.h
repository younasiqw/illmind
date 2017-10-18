#pragma once

#include "Utilities.h"
#include "SqchSDK\Interfaces.h"

class CFixMove {
public:
	void Start(SSDK::CUserCmd*);
	void Stop(SSDK::CUserCmd*);
	SSDK::QAngle GetOldAngle();
private:
	SSDK::QAngle old;
	float oldFowardMove;
	float oldSideMove;

	void CorrectMovement(SSDK::QAngle vOldAngles, SSDK::CUserCmd* pCmd, float fOldForward, float fOldSidemove);
};