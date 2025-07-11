#pragma once

#include "../Module.hpp"
#include "Events/Events.hpp"

class Clone : public Module {
private:
    bool clone = false;
    static std::vector<std::shared_ptr<Packet>> inFlightPackets;
public:
    Clone() : Module("Skin Cloner", "Equip someone else's skin",
        IDR_TEXT_BOX_PNG, "MB_MIDDLE") {
    };

    void onEnable() override;

    void onDisable() override;

    void defaultConfig() override;

    void settingsRender(float settingsOffset) override;

    void onTick(TickEvent& event);

    void onKey(KeyEvent& event);

    void onMouse(MouseEvent& event);
};
