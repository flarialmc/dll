#include "AutoPerspective.hpp"

#include "Events/EventManager.hpp"

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
    
}

void AutoPerspective::settingsRender(float settingsOffset) {

    initSettingsPage();


    addHeader("Auto Perspective");

    addToggle("Elytra", "Toggle to change perspective while using elytra to fly.", "elytra");
    addDropdown("Elytra Perspective", "Your perspective while using elytra to fly", std::vector<std::string>{"1st Person", "3rd Person back", "3rd Person front"}, "elytraMode", true);

    addToggle("Swimming", "Toggle to change perspective while swimming.", "swimming");
    addDropdown("Swimming Perspective", "Your perspective while swimming.", std::vector<std::string>{"1st Person", "3rd Person back", "3rd Person front"}, "swimmingMode", true);

    //addToggle("Riding", "Toggle to change perspective while riding a mob.", "riding");
    //addDropdown("Riding Perspective", "Your perspective while riding a mob", std::vector<std::string>{"1st Person", "3rd Person back", "3rd Person front"}, getOps<std::string>("ridingMode"));
    FlarialGUI::UnsetScrollView();

    resetPadding();
}

void AutoPerspective::onGetViewPerspective(PerspectiveEvent& event) {
        if (this->isEnabled()) {
            auto Player = SDK::clientInstance->getLocalPlayer();
            std::string Mode = "";

            if (false) { //Couldn't get this to work for some reason :heartbreak:
                if (getOps<bool>("Riding")) {

                    Mode = getOps<std::string>("ridingMode");

                    if (OriginalPerspective == -1) OriginalPerspective = (int)event.getPerspective();
                    else if ((int)event.getPerspective() != OriginalPerspective or Changed) {
                        Changed = true;
                        return;
                    }
                }
            }

            else if (Player->getActorFlag(ActorFlags::FLAG_GLIDING)) {
                if (getOps<bool>("elytra")) {

                    Mode = getOps<std::string>("elytraMode");

                    if (OriginalPerspective == -1) OriginalPerspective = (int)event.getPerspective();
                    else if ((int)event.getPerspective() != OriginalPerspective or Changed) {
                        Changed = true;
                        return;
                    }
                }
            }

            else if (Player->getActorFlag(ActorFlags::FLAG_SWIMMING)) {
                if (getOps<bool>("swimming")) {

                    Mode = getOps<std::string>("swimmingMode");

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