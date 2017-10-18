#pragma once

#include "SqchSDK\Interfaces.h"
#include "SqchSDK\Offsets.h"

#include "EntityStructs.h"
#include "OptionsManager.h"

#include "Utilities.h"

#define	HITGROUP_GENERIC	0
#define	HITGROUP_HEAD		1
#define	HITGROUP_CHEST		2
#define	HITGROUP_STOMACH	3
#define HITGROUP_LEFTARM	4	
#define HITGROUP_RIGHTARM	5
#define HITGROUP_LEFTLEG	6
#define HITGROUP_RIGHTLEG	7
#define HITGROUP_GEAR		10

struct FireBulletData
{
	FireBulletData(const SSDK::Vector &eye_pos)
		: src(eye_pos)
	{
	}

	SSDK::Vector           src;
	SSDK::Trace_t          enter_trace;
	SSDK::Vector           direction;
	SSDK::CTraceFilter    filter;
	float           trace_length;
	float           trace_length_remaining;
	float           current_damage;
	int             penetrate_count;
};

class CAutowall
{
public:
	static void UTIL_TraceLine(const SSDK::Vector& vecAbsStart, const SSDK::Vector& vecAbsEnd, unsigned int mask, CBaseEntity* ignore, SSDK::Trace_t* ptr);
	static void UTIL_ClipTraceToPlayers(SSDK::Vector& vecAbsStart, SSDK::Vector& vecAbsEnd, unsigned int mask, SSDK::ITraceFilter* filter, SSDK::Trace_t* tr);
	static void ScaleDamage(int hitgroup, CBaseEntity *enemy, float weapon_armor_ratio, float &current_damage);
	static bool TraceToExit(SSDK::Vector& end, SSDK::Trace_t& tr, float x, float y, float z, float x2, float y2, float z2, SSDK::Trace_t* trace);
	static bool TraceToExit1(SSDK::Vector &end, SSDK::Trace_t *enter_trace, SSDK::Vector start, SSDK::Vector dir, SSDK::Trace_t *exit_trace);
	static float GetHitgroupDamageMultiplier(int iHitGroup);
	static bool HandleBulletPenetration(SSDK::WeaponInfo_t* weaponInfo, FireBulletData &data);
	static bool SimulateFireBullet(CBaseCombatWeapon* pWeapon, FireBulletData &data);
	static float GetDamage(const SSDK::Vector& point);
	static bool CanDamage(const SSDK::Vector& point);


	//2
	static bool bTraceExit(SSDK::Vector &vStartyPosy, SSDK::Vector &vDir);
	static float Invasive(SSDK::Vector vEnd);

	static float BestHitPoint(CBaseEntity* pEntity, int prioritized, float minDmg, SSDK::mstudiohitboxset_t* hitset, SSDK::matrix3x4a_t matrix[], SSDK::Vector& vecOut);
	static SSDK::Vector CalculateBestPoint(CBaseEntity* pEntity, int prioritized, float minDmg, bool onlyPrioritized = true);
private:
	static SSDK::Trace_t trace;
	static SSDK::Trace_t trace2;
};