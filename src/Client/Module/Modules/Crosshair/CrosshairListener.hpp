#pragma once

#include "../../../Events/Listener.hpp"

class Crosshair;

class CrosshairListener : public Listener {
    Crosshair* mod;
    bool drawThirdPersonCrosshair;

public:
    void onEnable() {
    };
    void onDisable() {

    };

    explicit CrosshairListener(const std::string& name, Crosshair* mod) {
        this->name = name;
        this->mod = mod;
    }

    void onGetViewPerspective(PerspectiveEvent &event) override {
        //ThirdPersonBack = 1
        //ThirdPersonFront = 2
    }
    

    ~CrosshairListener() {
        onDisable();
    }
};
