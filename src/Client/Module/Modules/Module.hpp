#pragma once

#include "../../../Utils/Utils.hpp"
#include <string>

struct ModuleEvent
{
    bool isEnabled, isTicking;
};

class Module
{
public:
    std::string name;
    std::string description;
    std::string icon;
    int keybind;

public:
    bool enabled = false;

public:
    Module(std::string, std::string, std::string, uint64_t);

public:

    virtual void onEnable() {}

    virtual void onDisable() {}
};