#pragma once

#include "SqchSDK\Interfaces.h"
#include "SqchSDK\Offsets.h"

#include "EntityStructs.h"
#include "OptionsManager.h"
#include "Globals.h"

#include "Utilities.h"
#include "DrawManager.h"

/*
*
* Unused//NotWorking
*
*/

class CBulletListener
{
	class BulletListener : IGameEventListener2
	{
	public:
		void start()
		{
			if (!SSDK::I::GameEvents()->AddListener(this, "bullet_impact", false))
			{
				SSDK::I::CVar()->ConsoleColorPrintf(SSDK::Color(255, 0, 0), XorStr("Failed to register bullet impact listener. \n"));
			}
		}
		void stop()
		{
			SSDK::I::GameEvents()->RemoveListener(this);
		}
		void FireGameEvent(IGameEvent* event) override
		{
			CBulletListener::singleton()->OnFireEvent(event);
		}
		int GetEventDebugID(void) override
		{
			return EVENT_DEBUG_ID_INIT /*0x2A*/;
		}
	};
public:
	static CBulletListener* singleton()
	{
		static CBulletListener* instance = new CBulletListener();
		return instance;
	}

	void init()
	{
		_listener.start();
	}

	void OnFireEvent(IGameEvent* event)
	{
		if (!strcmp(event->GetName(), "bullet_impact"))
		{
			auto index = SSDK::I::Engine()->GetPlayerUserID(event->GetInt("userid"));

			if (index != SSDK::I::Engine()->GetLocalPlayer())
				return;

			auto local = (CBaseEntity*)SSDK::I::EntityList()->GetClientEntity(index);

			if (!local)
				return;

			SSDK::Vector position(event->GetFloat("x"), event->GetFloat("y"), event->GetFloat("z"));

			SSDK::Ray_t ray;
			ray.Init(local->GetEyePos(), position);

			SSDK::CTraceFilter filter;
			filter.pSkip = local;

			SSDK::Trace_t tr;
			SSDK::I::EngineTrace()->TraceRay(ray, MASK_SHOT, &filter, &tr);

			logs.push_back(cbullet_tracer_info(local->GetEyePos(), position, SSDK::I::GlobalVars()->curtime, SSDK::Color(140, 29, 29, 255)));
		}
	}

	/*void OnStudioRender()
	{
		if (!OptionsManager.bBulletTracers)
			return;

		auto local = (CBaseEntity*)SSDK::I::EntityList()->GetClientEntity(SSDK::I::Engine()->GetLocalPlayer());

		if (!local)
			return;

		for (size_t i = 0; i < logs.size(); i++)
		{
			auto current = logs.at(i);
			
			SSDK::I::DebugOverlay()->AddLineOverlay(current.src, current.dst, current.color.r(), current.color.g(), current.color.b(), true, -1.f);

			if (fabs(SSDK::I::GlobalVars()->curtime - current.time) > 3.f)
				logs.erase(logs.begin() + i);
		}
	}*/

private:
	BulletListener _listener;

	class cbullet_tracer_info
	{
	public:
		cbullet_tracer_info(SSDK::Vector src, SSDK::Vector dst, float time, SSDK::Color color)
		{
			this->src = src;
			this->dst = dst;
			this->time = time;
			this->color = color;
		}

		SSDK::Vector src, dst;
		float time;
		SSDK::Color color;
	};

	std::vector<cbullet_tracer_info> logs;
};