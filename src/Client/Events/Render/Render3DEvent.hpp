#pragma once

#include "../Event.hpp"
#include "SDK/Client/Level/LevelRender/LevelRender.hpp"
#include "SDK/Client/Render/ScreenContext.hpp"

class Render3DEvent : public Event {
public:
    LevelRender* LevelRenderer;
    ScreenContext* screenContext;
    explicit Render3DEvent(LevelRender* mLevelRenderer, ScreenContext* mScreenContext) : LevelRenderer(mLevelRenderer), screenContext(mScreenContext) {}
};


