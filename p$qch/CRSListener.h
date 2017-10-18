#include <Windows.h>

#include "SqchSDK\Interfaces.h"
#include "SqchSDK\Offsets.h"

#include "EntityStructs.h"
#include "OptionsManager.h"

#include "Utilities.h"

class CRSListener
{
	class RSListener : IGameEventListener2
	{
	public:
		void start()
		{
			if (!SSDK::I::GameEvents()->AddListener(this, "round_start", false))
			{
				SSDK::I::CVar()->ConsoleColorPrintf(SSDK::Color(255, 0, 0), XorStr("Failed to register round start listener. \n"));
			}
		}
		void stop()
		{
			SSDK::I::GameEvents()->RemoveListener(this);
		}
		void FireGameEvent(IGameEvent* event) override
		{
			CRSListener::singleton()->OnFireEvent(event);
		}
		int GetEventDebugID(void) override
		{
			return EVENT_DEBUG_ID_INIT /*0x2A*/;
		}
	};
public:
	static CRSListener* singleton()
	{
		static CRSListener* instance = new CRSListener;
		return instance;
	}

	void init()
	{
		_listener.start();
	}

	void OnFireEvent(IGameEvent* event)
	{
		if (!strcmp(event->GetName(), "round_start"))
		{
			/* ~~ Resest Resolver Variables ~~ */

			/* ~~ Reset Resolver Variables Here, Deleted as Removed Resolver ~~*/

			/* ~~ End of Resolver Variables ~~ */
		}
	}

private:
	RSListener   _listener;
};