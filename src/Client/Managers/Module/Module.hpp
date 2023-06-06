#pragma once

#include "../../../Utils/Utils.hpp"
#include <string>

struct ModuleEvent {
    bool isEnabled, isTicking;
};

class Module {
public:
    std::string name;
    std::string description;
    std::string icon;
    uint64_t keybind;
public:
    bool enabled = false;
public:
    Module(std::string, std::string, std::string, uint64_t);

public:
    virtual void onKey(uintptr_t key, bool held, bool &cancel) {}

    virtual void onMouse(MouseAction action, bool &cancel) {}

    // TODO: This shit

    /* virtual void onPacketSender(Packet *packet) {}

    virtual void onPacketHandler(Packet *packet) {}

    virtual void onTick(GameMode *gm) {} */

    virtual void onRender() {}

};