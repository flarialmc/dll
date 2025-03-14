#pragma once

#include "../Module.hpp"


class AutoPerspective : public Module {
public:
    AutoPerspective() : Module("Auto Perspective", "Changes your perspective on specific events.", IDR_CURSOR_PNG, "") {
        Module::setup();
    };

    void onEnable() override {
        Listen(this, PerspectiveEvent, &AutoPerspective::onGetViewPerspective)
        Module::onEnable();
    }

    void onDisable() override {
        Deafen(this, PerspectiveEvent, &AutoPerspective::onGetViewPerspective)
        Module::onDisable();
    }

    void defaultConfig() override { Module::defaultConfig();
        if (settings.getSettingByName<bool>("elytra") == nullptr) {
            settings.addSetting("elytra", false);
            settings.addSetting("elytraMode", (std::string)"3rd Person back");
        }

        if (settings.getSettingByName<bool>("swimming") == nullptr) {
            settings.addSetting("swimming", false);
            settings.addSetting("swimmingMode", (std::string)"3rd Person back");
        }

        if (settings.getSettingByName<bool>("riding") == nullptr) {
            settings.addSetting("riding", false);
            settings.addSetting("ridingMode", (std::string)"3rd Person back");
        }
    }

    void settingsRender(float settingsOffset) override {

        float x = Constraints::PercentageConstraint(0.019, "left");
        float y = Constraints::PercentageConstraint(0.10, "top");

        const float scrollviewWidth = Constraints::RelativeConstraint(0.12, "height", true);


        FlarialGUI::ScrollBar(x, y, 140, Constraints::SpacingConstraint(5.5, scrollviewWidth), 2);
        FlarialGUI::SetScrollView(x - settingsOffset, Constraints::PercentageConstraint(0.00, "top"),
            Constraints::RelativeConstraint(1.0, "width"),
            Constraints::RelativeConstraint(0.88f, "height"));
        this->addHeader("Perspective Settings");

        this->addToggle("Elytra", "Toggle to change perspective while using elytra to fly.", settings.getSettingByName<bool>("elytra")->value);
        this->addDropdown("Elytra Perspective", "Your perspective while using elytra to fly", std::vector<std::string>{"1st Person", "3rd Person back", "3rd Person front"}, settings.getSettingByName<std::string>("elytraMode")->value);

        this->addToggle("Swimming", "Toggle to change perspective while swimming.", settings.getSettingByName<bool>("swimming")->value);
        this->addDropdown("Swimming Perspective", "Your perspective while swimming.", std::vector<std::string>{"1st Person", "3rd Person back", "3rd Person front"}, settings.getSettingByName<std::string>("swimmingMode")->value);

        //this->addToggle("Riding", "Toggle to change perspective while riding a mob.", settings.getSettingByName<bool>("riding")->value);
        //this->addDropdown("Riding Perspective", "Your perspective while riding a mob", std::vector<std::string>{"1st Person", "3rd Person back", "3rd Person front"}, settings.getSettingByName<std::string>("ridingMode")->value);
        FlarialGUI::UnsetScrollView();

        this->resetPadding();
    }

    int OriginalPerspective = -1;
    bool Changed = false;

    void onGetViewPerspective(PerspectiveEvent& event) {
        if (this->isEnabled()) {
            auto Player = SDK::clientInstance->getLocalPlayer();
            std::string Mode = "";

            if (false) { //Couldn't get this to work for some reason :heartbreak:
                if (settings.getSettingByName<bool>("Riding")->value) {

                    Mode = settings.getSettingByName<std::string>("ridingMode")->value;

                    if (OriginalPerspective == -1) OriginalPerspective = (int)event.getPerspective();
                    else if ((int)event.getPerspective() != OriginalPerspective or Changed) {
                        Changed = true;
                        return;
                    }
                }
            }

            else if (Player->getActorFlag(ActorFlags::FLAG_GLIDING)) {
                if (settings.getSettingByName<bool>("elytra")->value) {

                    Mode = settings.getSettingByName<std::string>("elytraMode")->value;

                    if (OriginalPerspective == -1) OriginalPerspective = (int)event.getPerspective();
                    else if ((int)event.getPerspective() != OriginalPerspective or Changed) {
                        Changed = true;
                        return;
                    }
                }
            }

            else if (Player->getActorFlag(ActorFlags::FLAG_SWIMMING)) {
                if (settings.getSettingByName<bool>("swimming")->value) {

                    Mode = settings.getSettingByName<std::string>("swimmingMode")->value;

                    if (OriginalPerspective == -1) OriginalPerspective = (int)event.getPerspective();
                    else if ((int)event.getPerspective() != OriginalPerspective or Changed) {
                        Changed = true;
                        return;
                    }
                }
            }
            else {
                OriginalPerspective = -1;
                Changed = false;
            }

            if (Mode == "1st Person") {
                event.setPerspective(Perspective::FirstPerson);
            }
            if (Mode == "3rd Person back") {
                event.setPerspective(Perspective::ThirdPersonBack);
            }
            if (Mode == "3rd Person front") {
                event.setPerspective(Perspective::ThirdPersonFront);
            }
        }
    }
};
