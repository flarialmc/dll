#pragma once

#include <format>
#include "../../../Events/Listener.hpp"
#include "../../../Events/Input/KeyEvent.hpp"
#include "../../../../Utils/Logger/Logger.hpp"
#include "../Module.hpp"
#include "../../../../SDK/SDK.hpp"
#include <Windows.h>
#include "../../Manager.hpp"
#include "../../../../SDK/Client/Core/Options.hpp"

class ZoomListener : public Listener {

    Module *module;

    void onGetFOV(FOVEvent &event) override {
        auto fov = event.getFOV();
        if(fov == 70) return;
        realFov = fov;

        if(fisrtTime){ // so that it doesn't unzoom on module load
            currentZoomVal = fov;
            fisrtTime = false;
            return;
        }

        float animspeed = module->settings.getSettingByName<float>("anim")->value;
        bool disableanim = module->settings.getSettingByName<bool>("disableanim")->value;
        bool alwaysanim = module->settings.getSettingByName<bool>("alwaysanim")->value;

        /*
        if (module->active) {
            animationFinished = false;
            if (fov > 180) {
                currentZoomVal = disableanim ? fov + zoomValue : std::lerp(currentZoomVal, fov + zoomValue, animspeed * FlarialGUI::frameFactor);
            } else {
                currentZoomVal = disableanim ? zoomValue : std::lerp(currentZoomVal, zoomValue, animspeed * FlarialGUI::frameFactor);
            }
        } else {
            if ((!animationFinished || alwaysanim) && !disableanim) {
                // Only lerp if animation hasn't finished
                currentZoomVal = std::lerp(currentZoomVal, fov, animspeed * FlarialGUI::frameFactor);
                if (currentZoomVal == zoomValue || std::abs(fov - currentZoomVal) < animspeed + unzoomAnimDisableTreshold) { // when fov changes due to sprinting animation used to play
                    // Set animationFinished to true only when reaching original fov
                    animationFinished = true;
                }
            } else {
                // Once animation finished, set current zoom to fov
                currentZoomVal = fov;
            }
        }
        */

        event.setFOV(currentZoomVal);
    }



    void onGetSensitivity(SensitivityEvent &event) override {
        if (module->active) {
            if (!saved) {
                saved = true;
                currentSensitivity = event.getSensitivity();
            }
            // TODO: smoothstep
            event.setSensitivity(currentSensitivity - (currentSensitivity * (((realFov - (zoomValue - 1)) / realFov) / 1.0f)));
        } else if (saved) {
            saved = false;
        }
    }

    //TODO: RE CHECK
    void onMouse(MouseEvent &event) override {
        if (SDK::currentScreen == "hud_screen" || SDK::currentScreen == "f1_screen" || SDK::currentScreen == "zoom_screen")
            if (module->active) {
                //todo make it so that modules work together
                auto fovchanger = ModuleManager::getModule("FOV Changer");
                auto upsidedown = ModuleManager::getModule("Upside Down");

                if (event.getAction() == MouseAction::ScrollUp) {
                    if ((fovchanger != nullptr &&
                         fovchanger->settings.getSettingByName<float>("fovvalue")->value > 180) ||
                        (upsidedown != nullptr && upsidedown->isEnabled()))
                        zoomValue += this->module->settings.getSettingByName<float>("modifier")->value;
                    else zoomValue -= this->module->settings.getSettingByName<float>("modifier")->value;
                }
                if (event.getAction() != MouseAction::ScrollUp && event.getButton() == MouseButton::Scroll) {
                    if ((fovchanger != nullptr &&
                         fovchanger->settings.getSettingByName<float>("fovvalue")->value > 180) ||
                        (upsidedown != nullptr && upsidedown->isEnabled()))
                        zoomValue -= this->module->settings.getSettingByName<float>("modifier")->value;
                    else zoomValue += this->module->settings.getSettingByName<float>("modifier")->value;
                }

                if (zoomValue < 1) zoomValue = 1;
                else if (zoomValue > realFov) zoomValue = realFov;

                if (event.getAction() == MouseAction::ScrollUp ||
                        event.getAction() != MouseAction::ScrollUp && event.getButton() == MouseButton::Scroll) {
                    event.setButton(MouseButton::None);
                    event.setAction(MouseAction::Release);
                }
            }
    }

    void onKey(KeyEvent &event) override {
        if (module->isKeybind(event.keys) && module->isKeyPartOfKeybind(event.key)) {
            module->active = !module->active;
            if (!module->settings.getSettingByName<bool>("SaveModifier")->value) zoomValue = 30.0f;
        }

        if (!module->isKeybind(event.keys)) module->active = false;
    };
    // TODO: add dolly zoom and world2screen zoom stabilization ?
    // TODO: if someone has disabled hand this will enable it unless changed in settings
    void onSetTopScreenName(SetTopScreenNameEvent &event) override {
        auto hideHand = Options::getOption("hidehand");
        auto hideHud = Options::getOption("hidehud");
        if (module->active) {
            if (module->settings.getSettingByName<bool>("hidemodules")->value) {
                event.setCustomLayer("zoom_screen");
            }
//            if (module->settings.getSettingByName<bool>("hidehud")->value) {
//                event.setCustomLayer("zoom_screen");
//                if (hideHand != nullptr) hideHand->setvalue(true);
//                if (hideHud != nullptr) hideHud->setvalue(true);
//            }
            if(module->settings.getSettingByName<bool>("hidehand")->value){
                if(hideHand != nullptr) hideHand->setvalue(true);
            }
        } else {
            if (module->settings.getSettingByName<bool>("hidehand")->value) {
                if (hideHand != nullptr) hideHand->setvalue(false);
            }

//            if (module->settings.getSettingByName<bool>("hidehud")->value) { // TODO: there is a bug where it wont allow manual F1
//                if (hideHud != nullptr) hideHud->setvalue(false);
//            }

            if (hideHud != nullptr && hideHud->getvalue()) {
                event.setCustomLayer("f1_screen");
            }
        }
    }

public:
    static inline float currentZoomVal = 0.0f;
    static float inline zoomValue = 30.0f;
    static float inline realFov = 70.0f;
    static inline float currentSensitivity = 0.0f;
    static inline bool saved = false;

    static inline float unzoomAnimDisableTreshold = 0.2;

    static inline bool fisrtTime = true;

    static inline bool animationFinished = false;

    explicit ZoomListener(const char string[5], Module *module) {
        this->name = string;
        this->module = module;
    }
};
