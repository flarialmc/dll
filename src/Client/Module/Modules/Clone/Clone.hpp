#pragma once

#include "../Module.hpp"
#include "../../../../SDK/Client/Network/Packet/CommandRequestPacket.hpp"

class Clone : public Module {
private:
    bool clone = false;
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