#pragma once

#include "SqchSDK\Interfaces.h"
#include "SqchSDK\Offsets.h"

#include "EntityStructs.h"
#include "OptionsManager.h"

#include "Utilities.h"

namespace Globals
{
	extern bool FiredThatTick;

	extern int ShotsFired;
	extern int ShotsFiredEnt[65];
	extern int LoggedHits[65];
	extern int LoggedMisses[65];
}