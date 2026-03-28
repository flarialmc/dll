#pragma once

#include "../HUDModule.hpp"
#include "Events/Game/TickEvent.hpp"
#include "Events/Input/KeyEvent.hpp"
#include "Events/Render/RenderNameEvent.hpp"

class OpponentCPS : public HUDModule {
public:
    OpponentCPS() : HUDModule(5, "Opponent CPS", "Ability to show your Opponent's Click per second.",
        IDR_CPS_ICON_PNG, "O", {"cps", "opponent"}) {}

    void onSetup() override;

protected:
    std::string getDisplayValue() override;

    void customConfig() override;

    void customSettings() override;

    void customInit() override;

    void customCleanup() override;

private:
    void onRenderName(RenderNameEvent& event);
    
    void onKey(KeyEvent& event);
};
