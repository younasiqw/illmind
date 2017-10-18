#include "CAutoWall.h"


//1
void CAutowall::UTIL_TraceLine(const SSDK::Vector& vecAbsStart, const SSDK::Vector& vecAbsEnd, unsigned int mask, CBaseEntity* ignore, SSDK::Trace_t* ptr)
{
	SSDK::Ray_t ray;
	ray.Init(vecAbsStart, vecAbsEnd);
	SSDK::CTraceFilter filter;
	filter.pSkip = ignore;

	SSDK::I::EngineTrace()->TraceRay(ray, mask, &filter, ptr);
}

float DistanceToRay(const SSDK::Vector &pos, const SSDK::Vector &rayStart, const SSDK::Vector &rayEnd, float *along = NULL, SSDK::Vector *pointOnRay = NULL)
{
	SSDK::Vector to = pos - rayStart;
	SSDK::Vector dir = rayEnd - rayStart;
	float length = dir.NormalizeInPlace();

	float rangeAlong = dir.Dot(to);
	if (along)
	{
		*along = rangeAlong;
	}

	float range;

	if (rangeAlong < 0.0f)
	{
		// off start point
		range = -(pos - rayStart).Length();

		if (pointOnRay)
		{
			*pointOnRay = rayStart;
		}
	}
	else if (rangeAlong > length)
	{
		// off end point
		range = -(pos - rayEnd).Length();

		if (pointOnRay)
		{
			*pointOnRay = rayEnd;
		}
	}
	else // within ray bounds
	{
		SSDK::Vector onRay = rayStart + (dir.operator*(rangeAlong));
		range = (pos - onRay).Length();

		if (pointOnRay)
		{
			*pointOnRay = onRay;
		}
	}

	return range;
}

void CAutowall::UTIL_ClipTraceToPlayers(SSDK::Vector& vecAbsStart, SSDK::Vector& vecAbsEnd, unsigned int mask, SSDK::ITraceFilter* filter, SSDK::Trace_t* tr)
{
	SSDK::Trace_t playerTrace;
	SSDK::Ray_t ray;
	float smallestFraction = tr->fraction;
	const float maxRange = 60.0f;

	ray.Init(vecAbsStart, vecAbsEnd);

	for (int k = 1; k <= SSDK::I::GlobalVars()->maxClients; ++k)
	{
		CBaseEntity *player = (CBaseEntity*)SSDK::I::EntityList()->GetClientEntity(k);

		if (!player || !player->IsAlive())
			continue;

		if (player->IsDormant())
			continue;


		if (filter && filter->ShouldHitEntity((SSDK::IHandleEntity*)player, mask) == false)
			continue;

		float range = DistanceToRay(player->WorldSpaceCenter(), vecAbsStart, vecAbsEnd);
		if (range < 0.0f || range > maxRange)
			continue;

		SSDK::I::EngineTrace()->ClipRayToEntity(ray, mask | CONTENTS_HITBOX, (SSDK::IHandleEntity*)player, &playerTrace);
		if (playerTrace.fraction < smallestFraction)
		{
			// we shortened the ray - save off the trace
			*tr = playerTrace;
			smallestFraction = playerTrace.fraction;
		}
	}

}

float CAutowall::GetHitgroupDamageMultiplier(int iHitGroup)
{
	switch (iHitGroup)
	{
	case HITGROUP_HEAD:
		return 4.0f;
	case HITGROUP_CHEST:
	case HITGROUP_LEFTARM:
	case HITGROUP_RIGHTARM:
		return 1.0f;
	case HITGROUP_STOMACH:
		return 1.25f;
	case HITGROUP_LEFTLEG:
	case HITGROUP_RIGHTLEG:
		return 0.75f;
	default:
		return 1.0f;
	}
}

void CAutowall::ScaleDamage(int hitgroup, CBaseEntity *enemy, float weapon_armor_ratio, float &current_damage)
{
	current_damage *= GetHitgroupDamageMultiplier(hitgroup);

	if (enemy->GetArmor() > 0)
	{
		if (hitgroup == HITGROUP_HEAD)
		{
			if (enemy->HasHelmet())
				current_damage *= weapon_armor_ratio * 0.5f;
		}
		else
			current_damage *= weapon_armor_ratio * 0.5f;
	}
}

bool CAutowall::TraceToExit(SSDK::Vector& end, SSDK::Trace_t& tr, float x, float y, float z, float x2, float y2, float z2, SSDK::Trace_t* trace)
{
	typedef bool(__fastcall* TraceToExitFn)(SSDK::Vector&, SSDK::Trace_t&, float, float, float, float, float, float, SSDK::Trace_t*);
	static TraceToExitFn TraceToExit = (TraceToExitFn)SSDK::O::FindSignature(XorStr("client.dll"), XorStr("55 8B EC 83 EC 2C F3 0F 10 75 ? 33 C0"));

	if (!TraceToExit)
	{
		return false;
	}
	_asm
	{
		push trace
		push z2
		push y2
		push x2
		push z
		push y
		push x
		mov edx, tr
		mov ecx, end
		call TraceToExit
		add esp, 0x1C
	}
}

bool CAutowall::TraceToExit1(SSDK::Vector &end, SSDK::Trace_t *enter_trace, SSDK::Vector start, SSDK::Vector dir, SSDK::Trace_t *exit_trace)
{
	float distance = 0.0f;

	while (distance <= 90.0f)
	{
		distance += 4.0f;
		end = start + dir * distance;

		auto point_contents = SSDK::I::EngineTrace()->GetPointContents(end, MASK_SHOT_HULL | CONTENTS_HITBOX, NULL);

		if (point_contents & MASK_SHOT_HULL && !(point_contents & CONTENTS_HITBOX))
			continue;

		auto new_end = end - (dir * 4.0f);

		SSDK::Ray_t ray;
		ray.Init(end, new_end);
		SSDK::I::EngineTrace()->TraceRay(ray, MASK_SHOT, 0, exit_trace);

		if (exit_trace->startsolid && exit_trace->surface.flags & SURF_HITBOX)
		{
			ray.Init(end, start);

			SSDK::CTraceFilter filter;
			filter.pSkip = exit_trace->m_pEnt;

			SSDK::I::EngineTrace()->TraceRay(ray, 0x600400B, &filter, exit_trace);

			if ((exit_trace->fraction < 1.0f || exit_trace->allsolid) && !exit_trace->startsolid)
			{
				end = exit_trace->endpos;
				return true;
			}

			continue;
		}

		if (!(exit_trace->fraction < 1.0 || exit_trace->allsolid || exit_trace->startsolid) || exit_trace->startsolid)
		{
			if (exit_trace->m_pEnt)
			{
				if (enter_trace->m_pEnt && enter_trace->m_pEnt == SSDK::I::EntityList()->GetClientEntity(0))
					return true;
			}

			continue;
		}

		if (exit_trace->surface.flags >> 7 & 1 && !(enter_trace->surface.flags >> 7 & 1))
			continue;

		if (exit_trace->plane.normal.Dot(dir) <= 1.0f)
		{
			auto fraction = exit_trace->fraction * 4.0f;
			end = end - (dir * fraction);

			return true;
		}
	}

	return false;
}

bool CAutowall::HandleBulletPenetration(SSDK::WeaponInfo_t* weaponInfo, FireBulletData &data)
{
	SSDK::surfacedata_t *enter_surface_data = SSDK::I::PhysicsProps()->GetSurfaceData(data.enter_trace.surface.surfaceProps);
	int enter_material = enter_surface_data->game.material;
	float enter_surf_penetration_mod = *(float*)((uint8_t*)enter_surface_data + 76);

	data.trace_length += data.enter_trace.fraction * data.trace_length_remaining;
	data.current_damage *= powf(weaponInfo->flRangeModifier, data.trace_length * 0.002f);

	if (data.trace_length > 3000.f || enter_surf_penetration_mod < 0.1f)
		data.penetrate_count = 0;

	if (data.penetrate_count <= 0)
		return false;

	SSDK::Vector dummy;
	SSDK::Trace_t trace_exit;

	if (!TraceToExit1(dummy, &data.enter_trace, data.enter_trace.endpos, data.direction, &trace_exit))
		return false;

	SSDK::surfacedata_t *exit_surface_data = SSDK::I::PhysicsProps()->GetSurfaceData(trace_exit.surface.surfaceProps);
	int exit_material = exit_surface_data->game.material;

	float exit_surf_penetration_mod = *(float*)((uint8_t*)exit_surface_data + 76);
	float final_damage_modifier = 0.16f;
	float combined_penetration_modifier = 0.0f;

	if ((data.enter_trace.contents & CONTENTS_GRATE) != 0 || enter_material == 89 || enter_material == 71)
	{
		combined_penetration_modifier = 3.0f;
		final_damage_modifier = 0.05f;
	}
	else
		combined_penetration_modifier = (enter_surf_penetration_mod + exit_surf_penetration_mod) * 0.5f;

	if (enter_material == exit_material)
	{
		if (exit_material == 87 || exit_material == 85)
			combined_penetration_modifier = 3.0f;
		else if (exit_material == 76)
			combined_penetration_modifier = 2.0f;
	}

	float v34 = fmaxf(0.f, 1.0f / combined_penetration_modifier);
	float v35 = (data.current_damage * final_damage_modifier) + v34 * 3.0f * fmaxf(0.0f, (3.0f / weaponInfo->flPenetration) * 1.25f);
	float thickness = (trace_exit.endpos - data.enter_trace.endpos).Length();

	thickness *= thickness;
	thickness *= v34;
	thickness /= 24.0f;

	float lost_damage = fmaxf(0.0f, v35 + thickness);

	if (lost_damage > data.current_damage)
		return false;

	if (lost_damage >= 0.0f)
		data.current_damage -= lost_damage;

	if (data.current_damage < 1.0f)
		return false;

	data.src = trace_exit.endpos;
	data.penetrate_count--;

	return true;
}

bool CAutowall::SimulateFireBullet(CBaseCombatWeapon* pWeapon, FireBulletData &data)
{
	CBaseEntity* localplayer = CBaseEntity::GetLocalPlayer();
	SSDK::WeaponInfo_t* weaponInfo = pWeapon->GetCSWpnData();

	data.penetrate_count = 4;
	data.trace_length = 0.0f;
	data.current_damage = (float)weaponInfo->iDamage;

	while (data.penetrate_count > 0 && data.current_damage >= 1.0f)
	{
		data.trace_length_remaining = weaponInfo->flRange - data.trace_length;

		SSDK::Vector end = data.src + data.direction * data.trace_length_remaining;

		// data.enter_trace
		UTIL_TraceLine(data.src, end, MASK_SHOT, localplayer, &data.enter_trace);
		UTIL_ClipTraceToPlayers(data.src, end * 40.f, 0x4600400B, &data.filter, &data.enter_trace);

		if (data.enter_trace.fraction == 1.0f)
			break;

		if (data.enter_trace.hitgroup <= HITGROUP_RIGHTLEG && data.enter_trace.hitgroup > HITGROUP_GENERIC)
		{
			data.trace_length += data.enter_trace.fraction * data.trace_length_remaining;
			data.current_damage *= powf(weaponInfo->flRangeModifier, data.trace_length * 0.002f);

			CBaseEntity* player = (CBaseEntity*)data.enter_trace.m_pEnt;

			if (*player->GetTeamNum() == *localplayer->GetTeamNum())
				return false;

			ScaleDamage(data.enter_trace.hitgroup, player, weaponInfo->flArmorRatio, data.current_damage);

			return true;
		}

		if (!HandleBulletPenetration(weaponInfo, data))
			break;
	}

	return false;
}

float CAutowall::GetDamage(const SSDK::Vector& point)
{
	float damage = 0.f;
	SSDK::Vector dst = point;
	CBaseEntity* localplayer = CBaseEntity::GetLocalPlayer();
	FireBulletData data(localplayer->GetEyePos());
	data.filter.pSkip = localplayer;

	SSDK::QAngle angles = Utilities::CalcAngle(data.src, dst);
	Utilities::AngleVectors(angles, &data.direction);

	data.direction.NormalizeInPlace();

	CBaseCombatWeapon* activeWeapon = localplayer->GetActiveWeapon();
	if (!activeWeapon)
		return 0.f;

	if (SimulateFireBullet(activeWeapon, data)) {
		damage = data.current_damage;
	}

	return damage;
}

bool CAutowall::CanDamage(const SSDK::Vector& point)
{
	return GetDamage(point) > .0f;
}

//2
SSDK::Trace_t CAutowall::trace;
SSDK::Trace_t CAutowall::trace2;

bool CAutowall::bTraceExit(SSDK::Vector &vStartyPosy, SSDK::Vector &vDir)
{
	float multiply = 0;

	while (multiply < 90.1f)
	{
		multiply += 4.0f;

		auto vDaStart = (vDir * multiply) + vStartyPosy;
		auto vDaEnd = vDaStart - (vDir * 4.0f);

		auto point_contents = (SSDK::I::EngineTrace()->GetPointContents(vDaStart, 0x4600400B));

		SSDK::Ray_t ray2;

		if ((point_contents & 0x600400B) && (!(point_contents & 0x40000000)))
		{
			continue;
		}

		ray2.Init(vDaStart, vDaEnd);
		SSDK::I::EngineTrace()->TraceRay(ray2, 0x4600400B, nullptr, &trace2);

		if (trace2.startsolid && (trace2.surface.flags & 0x8000))
		{
			auto pNewTraceEnt = trace2.m_pEnt;

			SSDK::CSimpleTraceFilter tracefilter_new(pNewTraceEnt);

			SSDK::Ray_t ray3;
			ray3.Init(vDaStart, vStartyPosy);
			SSDK::I::EngineTrace()->TraceRay(ray3, 0x600400B, reinterpret_cast<SSDK::CTraceFilter*>(&tracefilter_new), &trace2);

			if ((trace2.fraction < 1.0f || trace2.allsolid || trace2.startsolid) && !trace2.startsolid)
			{
				vDaStart = trace2.endpos;
				return true;
			}
			continue;
		}

		if (!(trace2.fraction < 1.0f || trace2.allsolid || trace2.startsolid) || trace2.startsolid)
		{
			auto pBreakEnt = trace.m_pEnt;

			if (pBreakEnt)
			{
				auto pWorldEnt = SSDK::I::EntityList()->GetClientEntity(0);

				if (pBreakEnt != pWorldEnt)
				{
					static DWORD dwEntBreakable = NULL;

					if (dwEntBreakable == NULL)
					{
						dwEntBreakable = SSDK::O::FindSignature(XorStr("client.dll"), XorStr("55 8B EC 51 56 8B F1 85 F6 74 68"));
					}
					bool bRet;

					__asm
					{
						MOV ECX, pBreakEnt
						CALL dwEntBreakable
						MOV bRet, AL
					}
					if (bRet)
						return true;
				}
			}

			continue;
		}

		if (((trace2.surface.flags >> 7) & 1) && !((trace.surface.flags >> 7) & 1))
			continue;

		auto fWanker = ((trace2.plane.normal.x * vDir.x) + (trace2.plane.normal.y * vDir.y) + (trace2.plane.normal.z * vDir.z));

		if (fWanker < 1.1f)
		{
			auto fMody = trace2.fraction * 4.0f;

			auto vGody = vDaStart - (vDir * fMody);

			vDaStart = vGody;
			return true;
		}
	}
	return false;
}

float CAutowall::Invasive(SSDK::Vector vEnd)
{
	auto pWeapon = CBaseEntity::GetLocalPlayer()->GetActiveWeapon();
	if (!pWeapon) return 0.f;
	auto wiWeaponInfo = pWeapon->GetCSWpnData();
	auto vStart = CBaseEntity::GetLocalPlayer()->GetEyePos();

	static SSDK::Trace_t traceEmpty;

	trace = traceEmpty;
	static SSDK::CTraceFilterWorldAndPropsOnly tfNoPlayers;

	static auto pTraceFilter = &tfNoPlayers;

	SSDK::Vector vSource = vStart, vDir = (vEnd - vStart), vClip;
	vDir.NormalizeInPlace();

	float fTmpLength, fSumDist, fPow;

	float iCurrentDamage = (float)wiWeaponInfo->iDamage;


	auto fDamageCheck = 1.0f;

	auto iPenetration = 4;

	static DWORD dwCliptracetoplayers = NULL;

	if (dwCliptracetoplayers == NULL)
	{
		dwCliptracetoplayers = SSDK::O::FindSignature(XorStr("client.dll"), XorStr("53 8B DC 83 EC 08 83 E4 F0 83 C4 04 55 8B 6B 04 89 6C 24 04 8B EC 81 EC ? ? ? ? 8B 43 10"));
	}

	auto fRange = SSDK::Vector(vEnd - vStart).Length();

	while (iCurrentDamage > 0.0f)
	{
		SSDK::Ray_t ray;
		ray.Init(vSource, vEnd);

		SSDK::I::EngineTrace()->TraceRay(ray, 0x4600400B, pTraceFilter, &trace);

		vClip = (vDir * 40.0f) + vEnd;


		_asm
		{
			MOV EAX, pTraceFilter
			LEA ECX, trace
			PUSH ECX
			PUSH EAX
			PUSH 0x4600400B
			LEA EDX, vClip
			LEA ECX, vSource
			CALL dwCliptracetoplayers
			ADD ESP, 0x0C
		}


		if (trace.fraction != 1.0f)
		{

			auto pSurfaceData = SSDK::I::PhysicsProps()->GetSurfaceData(trace.surface.surfaceProps);

			auto material = *reinterpret_cast<USHORT*>(reinterpret_cast<DWORD>(pSurfaceData) + 84);
			auto fPenetrationPowerModifier = *reinterpret_cast<PFLOAT>(reinterpret_cast<DWORD>(pSurfaceData) + 76);

			if (iPenetration == 4)
				fTmpLength = fRange * trace.fraction;
			else
				fTmpLength = (fSumDist + ((fRange - fSumDist) * trace.fraction));

			fPow = (pow(wiWeaponInfo->flRangeModifier, (fTmpLength * 0.002f)));

			iCurrentDamage = iCurrentDamage * fPow;
			bool a5 = (trace.contents >> 3) & 1;

			bool v104 = (trace.surface.flags >> 7) & 1;

			auto v17 = false;

			if (iPenetration == 0 && !a5 && !v104 && material != 71 && material != 89)
				v17 = true;

			if (wiWeaponInfo->flPenetration < 0.0000001f || iPenetration < 1)
				v17 = true;

			trace2 = traceEmpty;

			auto vStartyPosy = trace.endpos;

			if (!bTraceExit(vStartyPosy, vDir)
				&& !(SSDK::I::EngineTrace()->GetPointContents(vStartyPosy, 0x600400B) & 0x600400B))
			{
				break;
			}

			if (v17)
			{
				break;
			}

			auto pSurfaceData2 = SSDK::I::PhysicsProps()->GetSurfaceData(trace2.surface.surfaceProps);

			auto material2 = *reinterpret_cast<USHORT*>(reinterpret_cast<DWORD>(pSurfaceData2) + 84);
			auto fPenetrationPowerModifer2 = *reinterpret_cast<PFLOAT>(reinterpret_cast<DWORD>(pSurfaceData2) + 76);

			auto v70 = 0.16f;
			float v31;

			if (a5 || v104)
			{
				if (material != 89 && material != 71)
					v31 = 1.0f;
				else
				{
					v31 = 3.0f;
					v70 = 0.05f;
				}
			}
			else
			{
				if (material != 89)
				{
					if (material != 71)
					{
						if (material == 70)
						{
							v31 = wiWeaponInfo->flPenetration;
						}
						else
						{

							v31 = (fPenetrationPowerModifer2 + fPenetrationPowerModifier) * 0.5f;
							v70 = 0.16f;
						}
					}
					else
					{
						v31 = 3.0f;
						v70 = 0.05f;
					}
				}
				else
				{
					v31 = 3.0f;
					v70 = 0.05f;
				}
			}

			if (material == material2)
			{
				if (material2 == 87 || material2 == 85)
					v31 = 3.0f;
				else
				{
					if (material2 == 76)
						v31 = 2.0f;
				}
			}

			auto v34 = 1.0f;
			v34 /= v31;

			if (v34 < 0.0f)
				v34 = 0.0f;

			auto fNewTmpLength = SSDK::Vector(trace2.endpos - trace.endpos).Length();

			auto fMine = 3.0f;
			fMine /= wiWeaponInfo->flPenetration;
			fMine *= 1.25f;

			auto v55 = 0.0f;

			if (fMine > 0.0f)
				v55 = fMine;

			auto v35 = v34;
			v35 *= 3.0f;
			v35 *= v55;

			v35 += (iCurrentDamage * v70);

			auto fShiz = fNewTmpLength;
			fShiz *= fNewTmpLength;
			fShiz *= v34;
			fShiz /= 24.0f;

			auto fBisc = (v35 + fShiz);

			auto v69 = 0.0f;

			if (fBisc > 0.0f)
				v69 = fBisc;

			auto fNewy = (iCurrentDamage - v69);

			iCurrentDamage = fNewy;

			if (fNewy < fDamageCheck)
				break;

			vSource = trace2.endpos;

			fSumDist = fTmpLength + fNewTmpLength;

			--iPenetration;
		}
		else
			return iCurrentDamage;
	}
	return 0.f;
}

float CAutowall::BestHitPoint(CBaseEntity* pEntity, int prioritized, float minDmg, SSDK::mstudiohitboxset_t* hitset, SSDK::matrix3x4a_t matrix[], SSDK::Vector& vecOut) {
	SSDK::mstudiobbox_t *hitbox = hitset->pHitbox(prioritized);

	if (!hitbox)
		return 0.f;

	std::vector<SSDK::Vector> vecArray;
	float flHigherDamage = 0.f;

	float mod = hitbox->m_flRadius != -1.f ? hitbox->m_flRadius : 0.f;

	SSDK::Vector max;
	SSDK::Vector min;
	Utilities::VectorTransform(hitbox->bbmax + mod, matrix[hitbox->bone], max);
	Utilities::VectorTransform(hitbox->bbmin - mod, matrix[hitbox->bone], min);

	auto center = (min + max) * 0.5f;

	SSDK::QAngle CurrentAngles = Utilities::CalcAngle(center, CBaseEntity::GetLocalPlayer()->GetEyePos());

	SSDK::Vector Forward;
	Utilities::AngleVectors(CurrentAngles, &Forward);

	SSDK::Vector Right = Forward.Cross(SSDK::Vector(0, 0, 1));
	SSDK::Vector Left = SSDK::Vector(-Right.x, -Right.y, Right.z);

	SSDK::Vector Top = SSDK::Vector(0, 0, 1);
	SSDK::Vector Bot = SSDK::Vector(0, 0, -1);

	static float POINT_SCALE = 0.74f;

	switch (prioritized) {
	case SSDK::HitboxEnum_t::HITBOX_HEAD:
		for (auto i = 0; i < 3; ++i) {
			vecArray.emplace_back(center);
		}
		vecArray[0] += Top * (hitbox->m_flRadius * POINT_SCALE);
		vecArray[1] += Right * (hitbox->m_flRadius * POINT_SCALE);
		vecArray[2] += Left * (hitbox->m_flRadius * POINT_SCALE);
		break;
	default:
		for (auto i = 0; i < 2; ++i) {
			vecArray.emplace_back(center);
		}
		vecArray[0] += Right * (hitbox->m_flRadius * POINT_SCALE);
		vecArray[1] += Left * (hitbox->m_flRadius * POINT_SCALE);
		break;
	}

	bool bVisPart = false;

	for (SSDK::Vector cur : vecArray) {
		float flCurDamage = GetDamage(cur);

		if (!flCurDamage)
			continue;

		bool bPrioritizeVis = OptionsManager.bPrioritizeVis;

		if (OptionsManager.aCacheAngle[pEntity->EntIndex()].bCustomSetts)
			bPrioritizeVis = OptionsManager.aCacheAngle[pEntity->EntIndex()].bPrioritizeVis;

		if (bPrioritizeVis) {
			if (pEntity->isVectorVisible(cur)) {
				bVisPart = true;
				if ((flCurDamage > flHigherDamage) && (flCurDamage > minDmg)) {
					flHigherDamage = flCurDamage;
					vecOut = cur;
				}
				continue;
			}
		}

		if (!bVisPart && (flCurDamage > flHigherDamage) && (flCurDamage > minDmg)) {
			flHigherDamage = flCurDamage;
			vecOut = cur;
		}
	}
	return flHigherDamage;
}

SSDK::Vector CAutowall::CalculateBestPoint(CBaseEntity* pEntity, int prioritized, float minDmg, bool onlyPrioritized) {
	SSDK::matrix3x4a_t matrix[128];

	if (!pEntity->SetupBones(matrix, 128, 256, SSDK::I::Engine()->GetLastTimeStamp()))
		return false;

	SSDK::studiohdr_t* hdr = SSDK::I::ModelInfo()->GetStudiomodel(pEntity->GetModel());
	SSDK::mstudiohitboxset_t* set = hdr->pHitboxSet(0);
	SSDK::Vector vecOutput;

	if (BestHitPoint(pEntity, prioritized, minDmg, set, matrix, vecOutput) > minDmg && onlyPrioritized) {
		return vecOutput;
	}
	else {
		float flHigherDamage = 0.f;
		SSDK::Vector vecCurVec;

		static int hitboxesLoop[] = {
			SSDK::HitboxEnum_t::HITBOX_HEAD,
			SSDK::HitboxEnum_t::HITBOX_LOWER_NECK,
			SSDK::HitboxEnum_t::HITBOX_UPPER_CHEST,
			SSDK::HitboxEnum_t::HITBOX_CHEST,
			SSDK::HitboxEnum_t::HITBOX_THORAX,
			SSDK::HitboxEnum_t::HITBOX_BODY,
			SSDK::HitboxEnum_t::HITBOX_LEFT_FOREARM,
			SSDK::HitboxEnum_t::HITBOX_RIGHT_FOREARM,
			SSDK::HitboxEnum_t::HITBOX_RIGHT_HAND,
			SSDK::HitboxEnum_t::HITBOX_LEFT_THIGH,
			SSDK::HitboxEnum_t::HITBOX_RIGHT_THIGH,
			SSDK::HitboxEnum_t::HITBOX_LEFT_CALF,
			SSDK::HitboxEnum_t::HITBOX_RIGHT_CALF,
			SSDK::HitboxEnum_t::HITBOX_LEFT_FOOT,
			SSDK::HitboxEnum_t::HITBOX_RIGHT_FOOT
		};

		int loopSize = ARRAYSIZE(hitboxesLoop);

		if (pEntity->isMoving() && !pEntity->isDucking()) {
			loopSize = 9;
		}

		bool bVisPart = false;

		for (int i = 0; i < loopSize; ++i) {
			if (!OptionsManager.aMultiHitboxes[i])
				continue;

			float flCurDamage = BestHitPoint(pEntity, hitboxesLoop[i], minDmg, set, matrix, vecCurVec);

			if (!flCurDamage)
				continue;

			bool bPrioritizeVis = OptionsManager.bPrioritizeVis;

			if (OptionsManager.aCacheAngle[pEntity->EntIndex()].bCustomSetts)
				bPrioritizeVis = OptionsManager.aCacheAngle[pEntity->EntIndex()].bPrioritizeVis;

			if (bPrioritizeVis) {
				if (pEntity->isVectorVisible(vecCurVec)) {
					bVisPart = true;
					if (flCurDamage > flHigherDamage) {
						flHigherDamage = flCurDamage;
						vecOutput = vecCurVec;
					}
					continue;
				}
			}

			if (!bVisPart && flCurDamage > flHigherDamage) {
				flHigherDamage = flCurDamage;
				vecOutput = vecCurVec;
			}
		}
		return vecOutput;
	}
}