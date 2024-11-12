#pragma once

#include "../Event.hpp"

class GeneralSettingsScreenControllerOnCreateEvent : public Event {
private:
    bool isGlobalSettings;
    bool reloadTexturePacksOnExit;
public:
    GeneralSettingsScreenControllerOnCreateEvent(bool isGlobalSettings, bool reloadTexturePacksOnExit) {
        this->isGlobalSettings = isGlobalSettings;
        this->reloadTexturePacksOnExit = reloadTexturePacksOnExit;
    };

    void unlockPackMenu() {
        isGlobalSettings = true;
        reloadTexturePacksOnExit = true;
    };

    bool getIsGlobalSettings() {
        return isGlobalSettings;
    };
    bool getReloadTexturePacksOnExit() {
        return reloadTexturePacksOnExit;
    };
};
