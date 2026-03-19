#pragma once

#include "../HUDModule.hpp"
#include "Events/Game/TickEvent.hpp"
#include "Events/Input/KeyEvent.hpp"
#include "Events/Input/MouseEvent.hpp"

class Sprint : public HUDModule {
public:
    Sprint() : HUDModule(5, "Toggle Sprint", "Automatically sprints for you!!!",
        IDR_AUTO_SPRINT_PNG, "CTRL", {"auto sprint", "autosprint"}) {}

    void onSetup() override;

    bool toggleSprinting = false;

protected:
    std::string getDisplayValue() override;

    void customConfig() override;

    void customSettings() override;

    void customInit() override;

    void customCleanup() override;

private:
    void onKey(KeyEvent& event);
    void onMouse(MouseEvent& event);
    void onTick(TickEvent& event);
};
