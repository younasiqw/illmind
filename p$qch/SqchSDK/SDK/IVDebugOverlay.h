#pragma once

#include "Vector.h"

namespace SSDK {
	class IVDebugOverlay {
	public:
		int ScreenPosition(const Vector& vIn, Vector& vOut)
		{
			typedef int(__thiscall* OrigFn)(void*, const Vector&, Vector&);
			return CallVFunction<OrigFn>(this, 13)(this, vIn, vOut);
		}

	/*	void AddLineOverlay(const Vector& origin, const Vector& dest, int r, int g, int b, bool noDepthTest, float duration) {
			typedef void(*oAddLineOverlay)(void*, const Vector&, const Vector&, int, int, int, bool, float);
			return CallVFunction<oAddLineOverlay>(this, 6)(this, origin, dest, r, g, b, noDepthTest, duration);
		}*/ // Broken
	};
}