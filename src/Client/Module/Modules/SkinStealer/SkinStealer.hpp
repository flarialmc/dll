#pragma once

#include "../Module.hpp"
#include "Events/Events.hpp"

class SkinStealer : public Module {
private:
    static std::vector<std::shared_ptr<Packet>> inFlightPackets;
public:
    bool clone = false;

    SkinStealer() : Module("Skin Stealer", "Equip someone else's skin",
        IDR_SKINSTEALER_PNG, "MB_MIDDLE", false, {"skin cloner"}) {
    };

    void onEnable() override;

    void onDisable() override;

    void defaultConfig() override;

    void settingsRender(float settingsOffset) override;

    static void cloneSkin(std::string targetName);

    void onTick(TickEvent& event);

    void onKey(KeyEvent& event);

    void onMouse(MouseEvent& event);
};
