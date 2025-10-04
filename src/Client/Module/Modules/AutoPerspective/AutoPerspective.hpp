#pragma once

#include "../Module.hpp"
#include "Events/Game/PerspectiveEvent.hpp"
#include "../../../../Assets/Assets.hpp"
#include "Events/Events.hpp"


class AutoPerspective : public Module {
public:
    AutoPerspective() : Module("Auto Perspective", "Changes your perspective on specific events.", IDR_PERSPECTIVE_PNG, "") {
        
    };

    void onEnable() override;

    void onDisable() override;

    void defaultConfig() override;

    void settingsRender(float settingsOffset) override;

    int OriginalPerspective = -1;
    bool Changed = false;

    void onGetViewPerspective(PerspectiveEvent& event);
};
