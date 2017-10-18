#include <Windows.h>
#include "SqchSDK\Interfaces.h"
#include "SqchSDK\Offsets.h"

#include "EntityStructs.h"
#include "OptionsManager.h"
#include "Globals.h"

#include "Utilities.h"
#pragma comment(lib, "Winmm.lib")

class CHurtListener
{
	class HurtListener : public IGameEventListener2
	{
	public:
		void start()
		{
			if (!SSDK::I::GameEvents()->AddListener(this, "player_hurt", false))
			{
				SSDK::I::CVar()->ConsoleColorPrintf(SSDK::Color(255, 0, 0), XorStr("Failed to register hurt listener. \n"));
			}
		}
		void stop()
		{
			SSDK::I::GameEvents()->RemoveListener(this);
		}
		void FireGameEvent(IGameEvent* event) override
		{
			CHurtListener::singleton()->OnFireEvent(event);
		}
		int GetEventDebugID(void) override
		{
			return EVENT_DEBUG_ID_INIT /*0x2A*/;
		}
	};
public:
	static CHurtListener* singleton()
	{
		static CHurtListener* instance = new CHurtListener;
		return instance;
	}
	
	void init()
	{
		_listener.start();
	}

	void OnFireEvent(IGameEvent* event)
	{
		if (!strcmp(event->GetName(), "player_hurt"))
		{
			int attacker = event->GetInt("attacker");
			int playerid = event->GetInt("userid");
			int damage = event->GetInt("dmg_health");
			int hitgroup = event->GetInt("hitgroup");

			CBaseEntity* pEntity = (CBaseEntity*)SSDK::I::EntityList()->GetClientEntity(SSDK::I::Engine()->GetPlayerUserID(playerid));
			
			if (SSDK::I::Engine()->GetPlayerUserID(attacker) == SSDK::I::Engine()->GetLocalPlayer())
			{
				/* ~~ Log hits for resolving ~~ */

				Globals::LoggedHits[pEntity->EntIndex()]++;

				/* ~~ Deleted as Resolver is Gone ~~ */

				/* ~~ End of Logging hits ~~*/


				/* ~~ Increase Hurt Timer ~~ */
				_flHurtTime = SSDK::I::GlobalVars()->curtime;
	
				auto curtime = SSDK::I::GlobalVars()->curtime;

				/* ~~ Play Skeet Sound ~~ */
				if (_flHurtTime + 0.25f >= curtime)
					SSDK::I::Engine()->ClientCmd_Unrestricted("play buttons\\arena_switch_press_02.wav");
			}
		}
	}
	
	/* ~~ Mark's Sexy Hitmarkers ~~ */

	void OnPaint()
	{
		auto curtime = SSDK::I::GlobalVars()->curtime;
		auto lineSize = 8;

		if (_flHurtTime + 0.25f >= curtime)
		{
			int screenSizeX, screenCenterX;
			int screenSizeY, screenCenterY;

			SSDK::I::Engine()->GetScreenSize(screenSizeX, screenSizeY);

			screenCenterX = screenSizeX / 2;
			screenCenterY = screenSizeY / 2;

			SSDK::I::MatSurface()->DrawSetColor(SSDK::Color(200, 200, 200, 255));
			SSDK::I::MatSurface()->DrawLine(screenCenterX - lineSize, screenCenterY - lineSize, screenCenterX - (lineSize / 4), screenCenterY - (lineSize / 4));
			SSDK::I::MatSurface()->DrawLine(screenCenterX - lineSize, screenCenterY + lineSize, screenCenterX - (lineSize / 4), screenCenterY + (lineSize / 4));
			SSDK::I::MatSurface()->DrawLine(screenCenterX + lineSize, screenCenterY + lineSize, screenCenterX + (lineSize / 4), screenCenterY + (lineSize / 4));
			SSDK::I::MatSurface()->DrawLine(screenCenterX + lineSize, screenCenterY - lineSize, screenCenterX + (lineSize / 4), screenCenterY - (lineSize / 4));
		}
	}

private:
	HurtListener _listener;
	float        _flHurtTime;
};