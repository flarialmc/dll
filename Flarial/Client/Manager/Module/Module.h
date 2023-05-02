#pragma once
#include "../Manager.h"
#include "Setting.h"

struct ModuleEvent {
    bool isEnabled, isTicking;
};

class Module {
public:
    std::vector<BaseSetting*> settings;
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
    virtual void onKey(uintptr_t key, bool held, bool& cancel) {}
    virtual void onMouse(MouseAction action, bool& cancel) {}
    virtual void onPacketSender(Packet* packet) {}
    virtual void onPacketHandler(Packet* packet) {}
    virtual void onTick(GameMode* gm) {}
    virtual void onRender() {}
public:
    void addBool(std::string name, std::string tooltip, bool* var, bool visible = true);
    void addFloat(std::string name, std::string tooltip, float min, float max, float* current, bool visible = true);
    void addColor(std::string name, std::string tooltip, float* color, bool visible = true);

    template <typename T>
    void addVec2(std::string name, std::string tooltip, Vec2<T>* vector, bool visible = true) {
        this->settings.push_back(new Vec2Setting<T>(name, tooltip, visible, vector));
    }
};