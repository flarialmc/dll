#include "AutoPerspective.hpp"
#include <GUI/Engine/EngineImGui.hpp>
#include "../../../../SDK/Client/Actor/LocalPlayer.hpp"

void AutoPerspective::onEnable() {
    Listen(this, PerspectiveEvent, &AutoPerspective::onGetViewPerspective)
    Module::onEnable();
}

void AutoPerspective::onDisable() {
    Deafen(this, PerspectiveEvent, &AutoPerspective::onGetViewPerspective)
    Module::onDisable();
}

void AutoPerspective::defaultConfig() {
    Module::defaultConfig("core");
    setDef("elytra", false);
    setDef("elytraMode", (std::string)"3rd Person back");
    setDef("swimming", false);
    setDef("swimmingMode", (std::string)"3rd Person back");
    setDef("riding", false);
    setDef("ridingMode", (std::string)"3rd Person back");
    setDef("emoting", false);
    setDef("emotingMode", (std::string)"3rd Person back");
    
}

void AutoPerspective::settingsRender(float settingsOffset) {

    initSettingsPage();

    addToggle("Elytra", "Toggle to change perspective while using elytra to fly.", "elytra");
    addConditionalDropdown(getOps<bool>("elytra"), "Elytra Perspective", "Your perspective while using elytra to fly", std::vector<std::string>{"1st Person", "3rd Person back", "3rd Person front"}, "elytraMode", true);

    addToggle("Swimming", "Toggle to change perspective while swimming.", "swimming");
    addConditionalDropdown(getOps<bool>("swimming"), "Swimming Perspective", "Your perspective while swimming.", std::vector<std::string>{"1st Person", "3rd Person back", "3rd Person front"}, "swimmingMode", true);

    addConditionalToggle(false, "Riding", "Toggle to change perspective while riding a mob.", "riding");
    addConditionalDropdown(false, "Riding Perspective", "Your perspective while riding a mob", std::vector<std::string>{"1st Person", "3rd Person back", "3rd Person front"}, "ridingMode", true);

    addToggle("Emoting", "Toggle to change perspective while emoting.", "emoting");
    addConditionalDropdown(getOps<bool>("emoting"), "Emoting Perspective", "Your perspective while emoting.", std::vector<std::string>{"1st Person", "3rd Person back", "3rd Person front"}, "emotingMode", true);
    FlarialGUI::UnsetScrollView();

    resetPadding();
}

void AutoPerspective::onGetViewPerspective(PerspectiveEvent& event) {
        if (this->isEnabled()) {
            auto Player = SDK::clientInstance->getLocalPlayer();
            std::string Mode = "";

            if (Player->getActorFlag(FLAG_RIDING)) { //Couldn't get this to work for some reason :heartbreak:
                if (getOps<bool>("Riding")) {

                    Mode = getOps<std::string>("ridingMode");

                    if (OriginalPerspective == -1) OriginalPerspective = (int)event.getPerspective();
                    else if ((int)event.getPerspective() != OriginalPerspective or Changed) {
                        Changed = true;
                        return;
                    }
                }
            }

            else if (Player->getActorFlag(FLAG_GLIDING)) {
                if (getOps<bool>("elytra")) {

                    Mode = getOps<std::string>("elytraMode");

                    if (OriginalPerspective == -1) OriginalPerspective = (int)event.getPerspective();
                    else if ((int)event.getPerspective() != OriginalPerspective or Changed) {
                        Changed = true;
                        return;
                    }
                }
            }

            else if (Player->getActorFlag(FLAG_SWIMMING)) {
                if (getOps<bool>("swimming")) {

                    Mode = getOps<std::string>("swimmingMode");

                    if (OriginalPerspective == -1) OriginalPerspective = (int)event.getPerspective();
                    else if ((int)event.getPerspective() != OriginalPerspective or Changed) {
                        Changed = true;
                        return;
                    }
                }
            }

            else if (Player->getActorFlag(FLAG_EMOTING))
            {
                if (getOps<bool>("emoting"))
                {
                    Mode = getOps<std::string>("emotingMode");
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

            if (Mode == "1st Person") event.setPerspective(Perspective::FirstPerson);
            if (Mode == "3rd Person back") event.setPerspective(Perspective::ThirdPersonBack);
            if (Mode == "3rd Person front") event.setPerspective(Perspective::ThirdPersonFront);
        }
    }
