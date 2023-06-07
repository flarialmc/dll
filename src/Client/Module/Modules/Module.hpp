#pragma once

#include "../../../Utils/Utils.hpp"
#include <string>

class Module
{
public:
    std::string name;
    std::string description;
    std::string icon;
    int keybind;

    Module(const std::string& ename, const std::string& edescription, const std::string& eicon, int ekey) {
        name = ename;
        description = edescription;
        icon = eicon;
        keybind = ekey;
    }
public:
    bool enabled = false;

public:

    virtual void onEnable() {}

    virtual void onDisable() {}
};