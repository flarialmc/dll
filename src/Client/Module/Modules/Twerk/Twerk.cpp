#include "Twerk.hpp"

#include "Events/EventManager.hpp"

void Twerk::onEnable() {
    Listen(this, TickEvent, &Twerk::onTick)
    Module::onEnable();
}

void Twerk::onDisable() {
    Deafen(this, TickEvent, &Twerk::onTick)
    Module::onDisable();
}

void Twerk::defaultConfig() {
    Module::defaultConfig("core");
    setDef("shiftInterval", 1.f);
    setDef("shiftSpeed", 0.5f);
}

void Twerk::settingsRender(float settingsOffset) {
    initSettingsPage();

    addHeader("Twerk");
    addSlider("Shift Interval", "Time in between consecutive twerks", "shiftInterval", 5.f, 0.1f);
    addSlider("Shift Speed", "Time between going up and down", "shiftSpeed", 5.f, 0.1f);

    FlarialGUI::UnsetScrollView();
    resetPadding();
}

void Twerk::onTick(TickEvent& event) {
    if (!this->isEnabled()) return;
    if (SDK::clientInstance != nullptr) {
        if (SDK::clientInstance->getLocalPlayer() != nullptr) {

            auto* handler = SDK::clientInstance->getLocalPlayer()->getMoveInputHandler();
            auto now = std::chrono::steady_clock::now();

            if (!ini) {
                lastShift = now;
                ini = true;
                return;
            }

            auto elapsedTime = static_cast<float>(std::chrono::duration_cast<std::chrono::milliseconds>(now - lastShift).count()) / 1000.f;

            if (
                (shiftCount % 2 == 0 && elapsedTime > getOps<float>("shiftInterval")) ||
                (shiftCount % 2 != 0 && elapsedTime > getOps<float>("shiftSpeed"))
            ) {
                handler->sneaking = !handler->sneaking;
                handler->mInputState.mSneakDown = !handler->mInputState.mSneakDown;
                handler->mRawInputState.mSneakDown = !handler->mRawInputState.mSneakDown;
                lastShift = now;
                shiftCount++;
            }

        }
    }
}
