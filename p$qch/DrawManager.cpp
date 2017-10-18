#include "DrawManager.h"

SSDK::HFont DrawManager::hFont = 0;
SSDK::HFont DrawManager::hFont2 = 0;
SSDK::HFont DrawManager::hFontWeapon = 0;
SSDK::HFont DrawManager::hFontLBY = 0;

static DrawManager DrawMgr = DrawManager();

void DrawManager::InitializeFont(const char* fontName, int size) {
	hFont = SSDK::I::MatSurface()->CreateFont();
	SSDK::I::MatSurface()->SetFontGlyphSet(hFont, fontName, size, size * 10, 0, 0, (int)SSDK::FontFlags::FONTFLAG_ANTIALIAS);

	hFont2 = SSDK::I::MatSurface()->CreateFont();
	SSDK::I::MatSurface()->SetFontGlyphSet(hFont2, fontName, size - 2, (size - 2) * 10, 0, 0, (int)SSDK::FontFlags::FONTFLAG_OUTLINE);

	hFontWeapon = SSDK::I::MatSurface()->CreateFont();
	SSDK::I::MatSurface()->SetFontGlyphSet(hFontWeapon, "Counter-Strike", 36, 0, 0, 0, (int)SSDK::FontFlags::FONTFLAG_OUTLINE);

	hFontLBY = SSDK::I::MatSurface()->CreateFont();
	SSDK::I::MatSurface()->SetFontGlyphSet(hFontLBY, "Tahoma", 12, FW_DEMIBOLD, NULL, NULL, (int)SSDK::FontFlags::FONTFLAG_DROPSHADOW);
}

void DrawManager::DrawString(SSDK::Color cColor, SSDK::HFont font, const char* chString, int x, int y) {
	wchar_t formated[128] = { '\0' };
	wsprintfW(formated, L"%S", chString);
	SSDK::I::MatSurface()->DrawSetTextPos(x, y);
	SSDK::I::MatSurface()->DrawSetTextColor(cColor);
	SSDK::I::MatSurface()->DrawSetTextFont(font);
	SSDK::I::MatSurface()->DrawPrintText(formated, wcslen(formated));
}

void DrawManager::DrawStringValue(SSDK::Color cColor, int x, int y, SSDK::HFont font, const char* chString, ...) {
	char buffer[128];
	va_list list;
	va_start(list, chString);
	vsprintf_s(buffer, chString, list);
	va_end(list);
	wchar_t formated[128] = { '\0' };
	wsprintfW(formated, L"%S", buffer);
	SSDK::I::MatSurface()->DrawSetTextPos(x, y);
	SSDK::I::MatSurface()->DrawSetTextColor(cColor);
	SSDK::I::MatSurface()->DrawSetTextFont(font);
	SSDK::I::MatSurface()->DrawPrintText(formated, wcslen(formated));
}

void DrawManager::DrawOutlinedRect(SSDK::Color cColor, float x0, float y0, float x1, float y1) {
	SSDK::I::MatSurface()->DrawSetColor(cColor);
	SSDK::I::MatSurface()->DrawOutlinedRect(x0, y0, x1, y1);
}

void DrawManager::DrawHealthBar(int health, float x0, float y0, float x1, float y1) {
	SSDK::I::MatSurface()->DrawSetColor(SSDK::Color(0, 0, 0, 210));
	SSDK::I::MatSurface()->DrawFilledRect(x0, y0, x1, y1);
	float width = x1 - x0;
	float calcWidth = width - ((width / 100) * health);
	SSDK::I::MatSurface()->DrawSetColor(SSDK::Color(((100 - health) / 100.0f)*255.0f, (health / 100.0f)*255.0f, 0, 210));
	SSDK::I::MatSurface()->DrawFilledRect(x0, y0, x1 - calcWidth, y1);
	DrawManager::DrawOutlinedRect(SSDK::Color(0, 0, 0), x0 - 1, y0 - 1, x1 + 1, y1 + 1);
}

void DrawManager::DrawNewHealthBar(CBaseEntity* entity, float bottom, float top, float left) {
	auto hp = entity->GetHealth();
	float box_h = (float)fabs(bottom - top);

	float off = 8;

	auto height = (((box_h * hp) / 100));

	int red = int(255 - (hp * 2.55f));
	int green = int(hp * 2.55f);

	int x = left - off;
	int y = top;
	int w = 4;
	int h = box_h;

	SSDK::I::MatSurface()->DrawSetColor(SSDK::Color(0, 0, 0, 255));
	SSDK::I::MatSurface()->DrawFilledRect(x, y, x + w, y + h);

	SSDK::I::MatSurface()->DrawSetColor(SSDK::Color(red, green, 0, 255));
	SSDK::I::MatSurface()->DrawFilledRect(x + 1, y + 1, x + w - 1, y + height - 2);
}

void DrawManager::DrawCrosshair()
{
	int w, h;

	SSDK::I::Engine()->GetScreenSize(w, h);
	SSDK::I::MatSurface()->DrawSetColor(SSDK::Color(239, 14, 14, 255));

	int cXinthechat = w / 2;
	int cY = h / 2;

	SSDK::I::MatSurface()->DrawLine(cXinthechat - 5, cY, cXinthechat + 5, cY);
	SSDK::I::MatSurface()->DrawLine(cXinthechat, cY - 5, cXinthechat, cY + 5);
}

void DrawManager::DrawBoxESP(CBaseEntity *pEntity, float x0, float y0, float x1, float y1) {
	switch (*pEntity->GetTeamNum()) {
	case 2:
		DrawManager::DrawOutlinedRect(SSDK::Color(234, 102, 50), x0, y0, x1, y1);
		DrawManager::DrawOutlinedRect(SSDK::Color(0, 0, 0, 255), x0 + 1.f, y0 + 1.f, x1 - 1.f, y1 - 1.f);
		DrawManager::DrawOutlinedRect(SSDK::Color(0, 0, 0, 255), x0 - 1.f, y0 - 1.f, x1 + 1.f, y1 + 1.f);
		SSDK::I::MatSurface()->DrawSetColor(SSDK::Color(234, 102, 50, 35));
		break;
	case 3:
		DrawManager::DrawOutlinedRect(SSDK::Color(50, 102, 234), x0, y0, x1, y1);
		DrawManager::DrawOutlinedRect(SSDK::Color(0, 0, 0, 255), x0 + 1.f, y0 + 1.f, x1 - 1.f, y1 - 1.f);
		DrawManager::DrawOutlinedRect(SSDK::Color(0, 0, 0, 255), x0 - 1.f, y0 - 1.f, x1 + 1.f, y1 + 1.f);
		SSDK::I::MatSurface()->DrawSetColor(SSDK::Color(50, 102, 234, 35));
		break;	
	default:
		break;
	}
}

void DrawManager::DrawArmorBar(int armor, float x0, float y0, float x1, float y1) {
	SSDK::I::MatSurface()->DrawSetColor(SSDK::Color(0, 0, 0, 210));
	SSDK::I::MatSurface()->DrawFilledRect(x0, y0, x1, y1);
	float width = x1 - x0;
	float calcWidth = width - ((width / 100) * armor);
	SSDK::I::MatSurface()->DrawSetColor(SSDK::Color(0, (armor / 100.0f)*102.0f, (armor / 100.0f)*204.0f, 210));
	SSDK::I::MatSurface()->DrawFilledRect(x0, y0, x1 - calcWidth, y1);
	DrawManager::DrawOutlinedRect(SSDK::Color(0, 0, 0), x0 - 1, y0 - 1, x1 + 1, y1 + 1);
}

void DrawManager::DrawPlayerName(CBaseEntity* entity, int entIndex, int x, int y) {

	auto hp = entity->GetHealth();

	int red = int(255 - (hp * 2.55f));
	int green = int(hp * 2.55f);

	SSDK::PlayerInfo entityInfos;
	SSDK::I::Engine()->GetPlayerInfo(entIndex, &entityInfos);

	wchar_t formated[128] = { '\0' };
	wsprintfW(formated, L"%S", entityInfos.szName);

	int tall;
	int size;
	SSDK::I::MatSurface()->GetTextSize(hFont2, formated, size, tall);
	x -= size / 2;
	y -= tall / 2;

	SSDK::I::MatSurface()->DrawSetTextPos(x, y);
	SSDK::I::MatSurface()->DrawSetTextColor(SSDK::Color(red, green, 0));
	SSDK::I::MatSurface()->DrawSetTextFont(hFont2);
	SSDK::I::MatSurface()->DrawPrintText(formated, wcslen(formated));
}

void DrawManager::DrawWeaponESP(SSDK::Color col, CBaseEntity *pEntity, float x, float y) {
	CBaseCombatWeapon *activeWeapon = pEntity->GetActiveWeapon();
	if (activeWeapon) {
		wchar_t formated[64] = { '\0' };

		int tall;
		int size;
		SSDK::I::MatSurface()->GetTextSize(hFontWeapon, formated, size, tall);

		x -= size / 2;
		y -= tall / 2;
		DrawStringValue(col, x, y, hFontWeapon, (char*)formated);
	}
}

void DrawManager::DrawIndex(SSDK::Color col, CBaseEntity *pEntity, float x, float y) {
	switch (*pEntity->GetTeamNum()) {
	case 2:
		DrawStringValue(col, x, y, hFont2, "Terrorist", pEntity->EntIndex());
		break;
	case 3:
		DrawStringValue(col, x, y, hFont2, "CT", pEntity->EntIndex());
		break;
	default:
		DrawStringValue(col, x, y, hFont2, "Spectators", pEntity->EntIndex());
		break;
	}
}

/*void DrawManager::DrawHitBoxESP(SSDK::Color cColor, CBaseEntity* pEnt) { // DEBOG
SSDK::matrix3x4a_t matrix[128];
if (!pEnt->SetupBones(matrix, 128, 256, SSDK::I::Engine()->GetLastTimeStamp()))
return;

SSDK::studiohdr_t* hdr = SSDK::I::ModelInfo()->GetStudiomodel(pEnt->GetModel());
SSDK::mstudiohitboxset_t* set = hdr->pHitboxSet(0);

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

for (int i = 0; i < 20; ++i) {
SSDK::mstudiobbox_t* hitbox = set->pHitbox(i);

if (!hitbox)
continue;

//SSDK::I::CVar()->ConsolePrintf("%f", hitbox->m_flRadius);

SSDK::Vector max;
SSDK::Vector min;
Utilities::VectorTransform(hitbox->bbmax, matrix[hitbox->bone], max);
Utilities::VectorTransform(hitbox->bbmin, matrix[hitbox->bone], min);

SSDK::Vector Bone2D;
SSDK::Vector center = (min + max) * 0.5f;


SSDK::QAngle viewAngle = Utilities::CalcAngle(center, CBaseEntity::GetLocalPlayer()->GetEyePos());

float pointscalex = 1.05f;
float pointscaley = 0.8f;

viewAngle.y += 90.f;
SSDK::Vector vEnd;
Utilities::AngleVectors(viewAngle, &vEnd);
vEnd = vEnd * (hitbox->m_flRadius * pointscaley) + center;
if (Utilities::W2S(vEnd, Bone2D)) {
DrawManager::DrawStringValue(SSDK::Color(255, 255, 255), Bone2D.x, Bone2D.y, DrawManager::hFont, "-x");
}

viewAngle = Utilities::CalcAngle(center, CBaseEntity::GetLocalPlayer()->GetEyePos());
viewAngle.y -= 89.f;
Utilities::AngleVectors(viewAngle, &vEnd);
vEnd = vEnd * (hitbox->m_flRadius * pointscaley) + center;
if (Utilities::W2S(vEnd, Bone2D)) {
DrawManager::DrawStringValue(SSDK::Color(255, 255, 255), Bone2D.x, Bone2D.y, DrawManager::hFont, "-x");
}

viewAngle = Utilities::CalcAngle(center, CBaseEntity::GetLocalPlayer()->GetEyePos());
viewAngle.x += 89.f;
Utilities::AngleVectors(viewAngle, &vEnd);
vEnd = vEnd * (hitbox->m_flRadius * pointscalex) + center;
if (Utilities::W2S(vEnd, Bone2D)) {
DrawManager::DrawStringValue(SSDK::Color(255, 255, 255), Bone2D.x, Bone2D.y, DrawManager::hFont, "-x");
}

viewAngle = Utilities::CalcAngle(center, CBaseEntity::GetLocalPlayer()->GetEyePos());
viewAngle.x -= 89.f;
Utilities::AngleVectors(viewAngle, &vEnd);
vEnd = vEnd * (hitbox->m_flRadius * pointscalex) + center;
if (Utilities::W2S(vEnd, Bone2D)) {
DrawManager::DrawStringValue(SSDK::Color(255, 255, 255), Bone2D.x, Bone2D.y, DrawManager::hFont, "-x");
}
}
}*/

void DrawManager::DrawSpread(CBaseEntity* pCSLocalPlayer, CBaseEntity* pCSTarget, CBaseCombatWeapon* pCSWeapon) {
	SSDK::Vector forward, right, up;
	SSDK::QAngle qAngle;
	SSDK::I::Engine()->GetViewAngles(qAngle);
	Utilities::AngleVectors(qAngle, &forward, &right, &up);

	pCSWeapon->UpdateAccuracy();

	for (auto i = 0; i < 100; ++i) {

		float fRand1 = Utilities::RandomFloat(0.f, 1.f);
		float fRandPi1 = Utilities::RandomFloat(0.f, 2.f * (float)M_PI);
		float fRand2 = Utilities::RandomFloat(0.f, 1.f);
		float fRandPi2 = Utilities::RandomFloat(0.f, 2.f * (float)M_PI);

		float fRandInaccuracy = fRand1 * pCSWeapon->GetAccuracy();
		float fRandSpread = fRand2 * pCSWeapon->GetSpread();

		float fSpreadX = cos(fRandPi1) * fRandInaccuracy + cos(fRandPi2) * fRandSpread;
		float fSpreadY = sin(fRandPi1) * fRandInaccuracy + sin(fRandPi2) * fRandSpread;

		SSDK::Vector vSpreadForward;
		vSpreadForward[0] = forward[0] + (fSpreadX * right[0]) + (fSpreadY * up[0]);
		vSpreadForward[1] = forward[1] + (fSpreadX * right[1]) + (fSpreadY * up[1]);
		vSpreadForward[2] = forward[2] + (fSpreadX * right[2]) + (fSpreadY * up[2]);
		vSpreadForward.NormalizeInPlace();

		SSDK::QAngle qViewAngles;
		Utilities::VectorAngles(vSpreadForward, qViewAngles);
		Utilities::Normalize(qViewAngles);

		qViewAngles += *pCSLocalPlayer->AimPunch() * 2.f;

		SSDK::QAngle vEnd(0, 0, 0);
		Utilities::AngleVectors(qAngle, &vEnd);

		vEnd = pCSLocalPlayer->GetEyePos() + (vEnd * 8192.f);

		SSDK::Vector D2A(0, 0, 0), D2B(0, 0, 0);
		if (Utilities::W2S(pCSLocalPlayer->GetEyePos(), D2A) &&
			Utilities::W2S(vEnd, D2B)) {
			SSDK::I::MatSurface()->DrawSetColor(SSDK::Color(0, 255, 0));
			SSDK::I::MatSurface()->DrawLine(D2A.x, D2A.y, D2B.x, D2B.y);
		}
	}
}

void DrawManager::DrawZoom() {
	int iScreenWidth, iScreenHeight;
	SSDK::I::Engine()->GetScreenSize(iScreenWidth, iScreenHeight);

	SSDK::I::MatSurface()->DrawSetColor(SSDK::Color(0, 0, 0));
	SSDK::I::MatSurface()->DrawLine(0, iScreenHeight / 2, iScreenWidth, iScreenHeight / 2);
	SSDK::I::MatSurface()->DrawLine(iScreenWidth / 2, 0, iScreenWidth / 2, iScreenHeight);
}

void DrawManager::DrawBoneLine(SSDK::Color cColor, CBaseEntity* pEntity, int p1, int p2) {
	SSDK::Vector screenPos1;
	SSDK::Vector screenPos2;
	if (Utilities::W2S(Utilities::GetEntityBone(pEntity, p1), screenPos1) &
		Utilities::W2S(Utilities::GetEntityBone(pEntity, p2), screenPos2)) {
		SSDK::I::MatSurface()->DrawSetColor(cColor);
		SSDK::I::MatSurface()->DrawLine(screenPos1.x, screenPos1.y, screenPos2.x, screenPos2.y);
	}
}

void DrawManager::DrawBoneESP(SSDK::Color cColor, CBaseEntity* pEnt) {
	SSDK::studiohdr_t* pStudioModel = SSDK::I::ModelInfo()->GetStudiomodel(pEnt->GetModel());
	if (pStudioModel)
	{
		for (int i = 0; i < pStudioModel->numbones; ++i)
		{
			SSDK::mstudiobone_t* pBone = pStudioModel->pBone(i);
			if (!pBone || !(pBone->flags & 256) || pBone->parent == -1)
				continue;

			DrawBoneLine(cColor, pEnt, i, pBone->parent);
		}
	}
}