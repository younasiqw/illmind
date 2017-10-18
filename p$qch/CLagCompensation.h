#pragma once

#include <math.h>

#include "SqchSDK\Interfaces.h"

#include "EntityStructs.h"
#include "Utilities.h"

#define TIME_TO_TICKS( dt )		( (int)( 0.5f + (float)(dt) / SSDK::I::GlobalVars()->interval_per_tick ) )
#define TICKS_TO_TIME( t )		( SSDK::I::GlobalVars()->interval_per_tick * ( t ) )

void StartLagCompensation(CBaseEntity* pEntity, SSDK::CUserCmd* cmd) {
	float flSimTime = *pEntity->GetSimulationTime();

	cmd->tick_count = TIME_TO_TICKS(flSimTime + 0.031f); // +1 Send
}