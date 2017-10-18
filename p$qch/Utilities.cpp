#include "Utilities.h"

#include <math.h>

bool Utilities::W2S(const SSDK::Vector& v3D, SSDK::Vector& v2D) {
	return (SSDK::I::DebugOverlay()->ScreenPosition(v3D, v2D) != 1);
}

SSDK::Vector Utilities::GetEntityBone(CBaseEntity* pEntity, int iBone) {
	SSDK::matrix3x4_t boneMatrix[128];

	if (!pEntity->SetupBones(boneMatrix, 128, 256, SSDK::I::Engine()->GetLastTimeStamp()))
		return SSDK::Vector();

	SSDK::matrix3x4_t hitbox = boneMatrix[iBone];

	return SSDK::Vector(hitbox[0][3], hitbox[1][3], hitbox[2][3]);
}

void Utilities::VectorTransform(SSDK::Vector& in1, SSDK::matrix3x4a_t& in2, SSDK::Vector &out) {
	out.x = in1.Dot(in2.m_flMatVal[0]) + in2.m_flMatVal[0][3];
	out.y = in1.Dot(in2.m_flMatVal[1]) + in2.m_flMatVal[1][3];
	out.z = in1.Dot(in2.m_flMatVal[2]) + in2.m_flMatVal[2][3];
}

void Utilities::SinCos(float radians, float *sine, float *cosine) {
	*sine = sin(radians);
	*cosine = cos(radians);
}

void Utilities::AngleVectors(const SSDK::QAngle &angles, SSDK::Vector *forward) {
	float sp, sy, cp, cy;

	SinCos(DEG2RAD(angles[SSDK::MathThings::YAW]), &sy, &cy);
	SinCos(DEG2RAD(angles[SSDK::MathThings::PITCH]), &sp, &cp);

	forward->x = cp*cy;
	forward->y = cp*sy;
	forward->z = -sp;
}

void Utilities::marquee(std::string& panicova_zlomena_noha)
{
	std::string temp_string = panicova_zlomena_noha;
	panicova_zlomena_noha.erase(0, 1);
	panicova_zlomena_noha += temp_string[0];
}



void Utilities::AngleVectors(const SSDK::QAngle &angles, SSDK::Vector *forward, SSDK::Vector *right, SSDK::Vector *up){
	float sr, sp, sy, cr, cp, cy;

	SinCos(DEG2RAD(angles[SSDK::MathThings::YAW]), &sy, &cy);
	SinCos(DEG2RAD(angles[SSDK::MathThings::PITCH]), &sp, &cp);
	SinCos(DEG2RAD(angles[SSDK::MathThings::ROLL]), &sr, &cr);

	if (forward)
	{
		forward->x = cp*cy;
		forward->y = cp*sy;
		forward->z = -sp;
	}

	if (right)
	{
		right->x = (-1 * sr*sp*cy + -1 * cr*-sy);
		right->y = (-1 * sr*sp*sy + -1 * cr*cy);
		right->z = -1 * sr*cp;
	}

	if (up)
	{
		up->x = (cr*sp*cy + -sr*-sy);
		up->y = (cr*sp*sy + -sr*cy);
		up->z = cr*cp;
	}
}

void Utilities::VectorAngles1(const SSDK::Vector& forward, SSDK::QAngle &angles){
	float	tmp, yaw, pitch;

	if (forward[1] == 0 && forward[0] == 0)
	{
		yaw = 0;
		if (forward[2] > 0)
			pitch = 270;
		else
			pitch = 90;
	}
	else
	{
		yaw = (atan2(forward[1], forward[0]) * 180 / M_PI);
		if (yaw < 0)
			yaw += 360;

		tmp = sqrt(forward[0] * forward[0] + forward[1] * forward[1]);
		pitch = (atan2(-forward[2], tmp) * 180 / M_PI);
		if (pitch < 0)
			pitch += 360;
	}

	angles[0] = pitch;
	angles[1] = yaw;
	angles[2] = 0;
}

void Utilities::VectorAngles(const SSDK::Vector& forward, SSDK::QAngle &angles) {
	if (forward[1] == 0.0f && forward[0] == 0.0f)
	{
		angles[0] = (forward[2] > 0.0f) ? 270.0f : 90.0f;
		angles[1] = 0.0f;
	}
	else
	{
		angles[0] = atan2(-forward[2], forward.Length2D()) * -180 / M_PI;
		angles[1] = atan2(forward[1], forward[0]) * 180 / M_PI;

		if (angles[1] > 90) angles[1] -= 180;
		else if (angles[1] < 90) angles[1] += 180;
		else if (angles[1] == 90) angles[1] = 0;
	}

	angles[2] = 0.0f;
}

//Better CalcAngle then v1
SSDK::QAngle Utilities::CalcAngle(SSDK::Vector src, SSDK::Vector dst) {
	SSDK::QAngle angles;
	SSDK::Vector delta = src - dst;
	VectorAngles(delta, angles);
	Normalize(delta);
	return angles;
}

bool Utilities::Clamp(SSDK::QAngle &angles) {
	SSDK::QAngle a = angles;
	Normalize(a);
	ClampAngles(a);

	if (isnan(a.x) || isinf(a.x) ||
		isnan(a.y) || isinf(a.y) ||
		isnan(a.z) || isinf(a.z)) {
		return false;
	}
	else {
		angles = a;
		return true;
	}
}

void Utilities::Normalize(SSDK::QAngle& angle) {
	while (angle.x > 89.0f) {
		angle.x -= 180.f;
	}
	while (angle.x < -89.0f) {
		angle.x += 180.f;
	}
	while (angle.y > 180.f) {
		angle.y -= 360.f;
	}
	while (angle.y < -180.f) {
		angle.y += 360.f;
	}
}

float Utilities::Normalize(float value)
{
	while (value > 180)
		value -= 360.f;

	while (value < -180)
		value += 360.f;
	return value;
}

void Utilities::Normalize(float& angle) {
	while (angle > 180.f) {
		angle -= 360.f;
	}
	while (angle < -180.f) {
		angle += 360.f;
	}
}

void Utilities::ClampAngles(SSDK::QAngle &angles) {
	if (angles.y > 180.0f)
		angles.y = 180.0f;
	else if (angles.y < -180.0f)
		angles.y = -180.0f;

	if (angles.x > 89.0f)
		angles.x = 89.0f;
	else if (angles.x < -89.0f)
		angles.x = -89.0f;

	angles.z = 0;
}

float Utilities::ClampYaw(float yaw) {
	while (yaw > 180.f)
		yaw -= 360.f;
	while (yaw < -180.f)
		yaw += 360.f;
	return yaw;
}

SSDK::Vector Utilities::extrapolateTick(SSDK::Vector p0, CBaseEntity* ent) {
	return p0 + (*ent->GetVelocity() * SSDK::I::GlobalVars()->interval_per_tick);
}

void Utilities::SetClanTag(const char* tag, const char* name) {
	using SetClanTagFn = void(__fastcall*) (const char *tag, const char *name);
	static SetClanTagFn SetClanTagEx = (SetClanTagFn)(SSDK::O::FindSignature(XorStr("engine.dll"), XorStr("53 56 57 8B DA 8B F9 FF 15")));
	SetClanTagEx(tag, name);
}

float Utilities::RandomFloat(float min, float max)
{
	typedef float(*RandomFloat_t)(float, float);
	static RandomFloat_t m_RandomFloat = (RandomFloat_t)GetProcAddress(GetModuleHandle(XorStr("vstdlib.dll")), XorStr("RandomFloat"));
	return m_RandomFloat(min, max);
}

void Utilities::RandomSeed(int iSeed)
{
	typedef void(*RandomSeed_t)(int);
	static RandomSeed_t m_RandomSeed = (RandomSeed_t)GetProcAddress(GetModuleHandle(XorStr("vstdlib.dll")), XorStr("RandomSeed"));
	return m_RandomSeed(iSeed);
}

template< class T, class Y >
T Utilities::ClampValue(T const &val, Y const &minVal, Y const &maxVal)
{
	if (val < minVal)
		return minVal;
	else if (val > maxVal)
		return maxVal;
	else
		return val;
}

float Utilities::GetInterp()
{
	float updaterate = SSDK::I::CVar()->FindVar(XorStr("cl_updaterate"))->GetFloat();
	SSDK::ConVar* minupdate = SSDK::I::CVar()->FindVar(XorStr("sv_minupdaterate"));
	SSDK::ConVar* maxupdate = SSDK::I::CVar()->FindVar(XorStr("sv_maxupdaterate"));

	if (minupdate && maxupdate)
		updaterate = maxupdate->GetFloat();

	float ratio = SSDK::I::CVar()->FindVar(XorStr("cl_interp_ratio"))->GetFloat();

	if (ratio == 0)
		ratio = 1.0f;

	float lerp = SSDK::I::CVar()->FindVar(XorStr("cl_interp"))->GetFloat();
	SSDK::ConVar* cmin = SSDK::I::CVar()->FindVar(XorStr("sv_client_min_interp_ratio"));
	SSDK::ConVar* cmax = SSDK::I::CVar()->FindVar(XorStr("sv_client_max_interp_ratio"));

	if (cmin && cmax && cmin->GetFloat() != 1)
		ratio = ClampValue(ratio, cmin->GetFloat(), cmax->GetFloat());

	return max(lerp, ratio / updaterate);
}

float Utilities::GetNetworkLatency()
{
	// Get true latency
	SSDK::INetChannelInfo *nci = SSDK::I::Engine()->GetNetChannelInfo();
	if (nci)
	{
		float OutgoingLatency = nci->GetAvgLatency(0);
		return OutgoingLatency;
	}
	else
	return 0.0f;
}

bool Utilities::HitChance(CBaseEntity* pCSLocalPlayer, CBaseEntity* pCSTarget, CBaseCombatWeapon* pCSWeapon, SSDK::QAngle qAngle, int seed, int chance) {
	if (!chance)
		return true;

	int iHit = 0;
	int iHitsNeed = (int)((float)OptionsManager.iHitChanceSeed * ((float)chance / 100.f));
	bool bHitchance = false;

	SSDK::Vector forward, right, up;
	AngleVectors(qAngle, &forward, &right, &up);

	pCSWeapon->UpdateAccuracy();

	for (auto i = 0; i < OptionsManager.iHitChanceSeed; ++i) {
		//Works better without seed lel
		//RandomSeed((seed & 0xFF) + 1);

		float RandomA = RandomFloat(0.0f, 1.0f);
		float RandomB = 1.0f - RandomA * RandomA;

		RandomA = RandomFloat(0.0f, M_PI_F * 2.0f);
		RandomB *= pCSWeapon->GetCone();

		float SpreadX1 = (cos(RandomA) * RandomB);
		float SpreadY1 = (sin(RandomA) * RandomB);

		float RandomC = RandomFloat(0.0f, 1.0f);
		float RandomF = RandomF = 1.0f - RandomC * RandomC;

		RandomC = RandomFloat(0.0f, M_PI_F * 2.0f);
		RandomF *= pCSWeapon->GetSpread();

		float SpreadX2 = (cos(RandomC) * RandomF);
		float SpreadY2 = (sin(RandomC) * RandomF);

		float fSpreadX = SpreadX1 + SpreadX2;
		float fSpreadY = SpreadY1 + SpreadY2;

		SSDK::Vector vSpreadForward;
		vSpreadForward[0] = forward[0] + (fSpreadX * right[0]) + (fSpreadY * up[0]);
		vSpreadForward[1] = forward[1] + (fSpreadX * right[1]) + (fSpreadY * up[1]);
		vSpreadForward[2] = forward[2] + (fSpreadX * right[2]) + (fSpreadY * up[2]);
		vSpreadForward.NormalizeInPlace();

		SSDK::QAngle qaNewAngle;
		VectorAngles1(vSpreadForward, qaNewAngle);
		Utilities::Normalize(qaNewAngle);

		//DEBUG
		//SSDK::I::Engine()->SetViewAngles(qaNewAngle);

		SSDK::QAngle vEnd;
		Utilities::AngleVectors(qaNewAngle, &vEnd);

		vEnd = pCSLocalPlayer->GetEyePos() + (vEnd * 8192.f);

		if (CBaseEntity::GetLocalPlayer()->canHit(vEnd, pCSTarget)) {
			iHit++;
		}

		if ((int)(((float)iHit / OptionsManager.iHitChanceSeed) * 100.f) >= chance) {
			bHitchance = true;
			break;
		}

		if ((OptionsManager.iHitChanceSeed - 1 - i + iHit) < iHitsNeed)
			break;
	}

	return bHitchance;
}

bool Utilities::CircleStrafer(SSDK::CUserCmd* pCmd, SSDK::Vector& vecOriginalView) {
	CBaseEntity* pLocalEntity = CBaseEntity::GetLocalPlayer();

	if (!pLocalEntity->isValidPlayer(true))
		return false;

	static int iCircleFact = 0;
	iCircleFact++;
	if (iCircleFact > 361)
		iCircleFact = 0;

	float flRotation = 3.f * iCircleFact - SSDK::I::GlobalVars()->interval_per_tick;

	SSDK::Vector StoredViewAngles = pCmd->viewangles;

	if (GetAsyncKeyState(VK_MBUTTON)) {
		pCmd->forwardmove = 450.f;
		pCmd->viewangles = vecOriginalView;
		flRotation = DEG2RAD(flRotation);

		float cos_rot = cos(flRotation);
		float sin_rot = sin(flRotation);

		float new_forwardmove = (cos_rot * pCmd->forwardmove) - (sin_rot * pCmd->sidemove);
		float new_sidemove = (sin_rot * pCmd->forwardmove) + (cos_rot * pCmd->sidemove);

		pCmd->forwardmove = new_forwardmove;
		pCmd->sidemove = new_sidemove;
	}

	return true;
}


int Utilities::FakelagCompensationBreak() {
	CBaseEntity *pLocalPlayer = CBaseEntity::GetLocalPlayer();
	SSDK::Vector velocity = *pLocalPlayer->GetVelocity();
	velocity.z = 0.f;
	float speed = velocity.Length();
	if (speed > 0.f) {
		auto distance_per_tick = speed *
			SSDK::I::GlobalVars()->interval_per_tick;
		int choked_ticks = std::ceilf(65.f / distance_per_tick);
		return std::min<int>(choked_ticks, 14);
	}
	return 1;
}