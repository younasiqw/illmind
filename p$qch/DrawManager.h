#pragma once

#include "SqchSDK\Interfaces.h"
#include "EntityStructs.h"
#include "Utilities.h"
#include "CAutoWall.h"

class DrawManager {
public:
	CBaseEntity* pTarget;

	static SSDK::HFont hFont;
	static SSDK::HFont hFont2;
	static SSDK::HFont hFontWeapon;
	static SSDK::HFont hFontLBY;

	static void InitializeFont(const char* fontName, int size);
	static void DrawString(SSDK::Color cColor, SSDK::HFont font, const char* chString, int x, int y);
	static void DrawStringValue(SSDK::Color cColor, int x, int y, SSDK::HFont font, const char* chString, ...);
	static void DrawOutlinedRect(SSDK::Color cColor, float x0, float y0, float x1, float y1);
	static void DrawHealthBar(int health, float x0, float y0, float x1, float y1);
	static void DrawNewHealthBar(CBaseEntity * entity, float bottom, float top, float left);
	static void DrawCrosshair();
	static void DrawBoxESP(CBaseEntity *pEntity, float x0, float y0, float x1, float y1);
	static void DrawArmorBar(int armor, float x0, float y0, float x1, float y1);
	static void DrawPlayerName(CBaseEntity * entity, int entIndex, int x, int y);
	static void DrawWeaponESP(SSDK::Color col, CBaseEntity *pEntity, float x, float y);
	static void DrawIndex(SSDK::Color col, CBaseEntity *pEntity, float x, float y);
	static void DrawSpread(CBaseEntity* pCSLocalPlayer, CBaseEntity* pCSTarget, CBaseCombatWeapon* pCSWeapon);
	static void DrawZoom();
	static void DrawBoneLine(SSDK::Color cColor, CBaseEntity* pEntity, int p1, int p2);
	static void DrawBoneESP(SSDK::Color cColor, CBaseEntity* pEnt);
};