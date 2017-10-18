#include "PT.h"

#include "CHurtListener.h"
#include "CBulletListener.h"
#include "Globals.h"

namespace PT {
	PaintTraverse_t g_fnOriginalPaintTraverse;
	void __fastcall Hooked_PaintTraverse(void* thisptr, void* edx, unsigned int vguiPanel, bool forceRepaint, bool allowForce) {
		static unsigned int iSelectedPanelZoom = 0;
		if (!iSelectedPanelZoom) {
			if (strstr(SSDK::I::VGUIPanel()->GetName(vguiPanel), XorStr("HudZoom")))
				iSelectedPanelZoom = vguiPanel;
		}
		else if (vguiPanel == iSelectedPanelZoom) {
			return;
		}

		g_fnOriginalPaintTraverse(thisptr, vguiPanel, forceRepaint, allowForce);

		static unsigned int iSelectedPanelMat = 0;

		if (!iSelectedPanelMat) {
			if (strstr(SSDK::I::VGUIPanel()->GetName(vguiPanel), XorStr("FocusOverlayPanel")))
				iSelectedPanelMat = vguiPanel;
		}
		else if (vguiPanel == iSelectedPanelMat) {
			if (SSDK::I::Engine()->isInGame()) {
				if (OptionsManager.bNoScope) {
					CBaseEntity *pLocalPlayer = CBaseEntity::GetLocalPlayer();
					if (pLocalPlayer->isValidPlayer(true)) {
						if (pLocalPlayer->isScoped()) {
							DrawManager::DrawZoom();
						}
					}
				}

				CHurtListener::singleton()->OnPaint();

				CBaseEntity *pLocalPlayer = CBaseEntity::GetLocalPlayer();

				for (int i = 0; i < SSDK::I::EntityList()->GetHighestEntityIndex(); ++i) {
					//if (SSDK::O::GlowManager()->m_GlowObjectDefinitions[i].IsEmpty() || !SSDK::O::GlowManager()->m_GlowObjectDefinitions[i].getEntity())
						//continue;

					CBaseEntity *pEntity = (CBaseEntity*)SSDK::I::EntityList()->GetClientEntity(i);

					if (!pEntity)
						continue;

					if (OptionsManager.bEspEnabled && pEntity->isValidPlayer()) {
						//ESP
						SSDK::Vector vecHeadHitBox = pEntity->GetEntityHitbox(0), vecOrigin = *pEntity->GetOrigin();
						SSDK::Vector vecHeadHitBox2D, vecOrigin2D;

						vecHeadHitBox.z += 10.f;

						if (Utilities::W2S(vecHeadHitBox, vecHeadHitBox2D) &&
							Utilities::W2S(vecOrigin, vecOrigin2D)) {
							float height = vecOrigin2D.y - vecHeadHitBox2D.y;
							float width = height * 0.45f;

							if (OptionsManager.bBoxESP)
								DrawManager::DrawBoxESP(pEntity, vecHeadHitBox2D.x - (width * 0.75), vecHeadHitBox2D.y - (height * 0.2), vecHeadHitBox2D.x + (width * 0.75), vecHeadHitBox2D.y + (height * 1.2));
							if (OptionsManager.bPlayerName)
								DrawManager::DrawPlayerName(pEntity, pEntity->EntIndex(), vecHeadHitBox2D.x, vecHeadHitBox2D.y - (height * 0.4));
							if (OptionsManager.bBones)
								DrawManager::DrawBoneESP(SSDK::Color(255, 255, 255, 255), pEntity);
						}
						if (CBaseEntity::GetLocalPlayer()->IsAlive()) {
							static SSDK::ConVar *viewmodel_fov = SSDK::I::CVar()->FindVar(XorStr("viewmodel_fov"));
							viewmodel_fov->m_fnChangeCallbacks.m_Size = 0;
							viewmodel_fov->SetValue(OptionsManager.iFov);
						}
					}
				}
			}
		}
	}
}