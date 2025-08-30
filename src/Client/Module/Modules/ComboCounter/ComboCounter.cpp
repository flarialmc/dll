#include "ComboCounter.hpp"

#include "Events/EventManager.hpp"
#include "Events/Game/TickEvent.hpp"
#include "Events/Render/RenderEvent.hpp"

void ComboCounter::onEnable() {
    Listen(this, AttackEvent, &ComboCounter::onAttack)
    Listen(this, TickEvent, &ComboCounter::onTick)
    Listen(this, RenderEvent, &ComboCounter::onRender)
    Module::onEnable();
}

void ComboCounter::onDisable() {
    Deafen(this, AttackEvent, &ComboCounter::onAttack)
    Deafen(this, TickEvent, &ComboCounter::onTick)
    Deafen(this, RenderEvent, &ComboCounter::onRender)
    Module::onDisable();
}

void ComboCounter::defaultConfig() {
    setDef("text", (std::string)"Combo: {value}");
    Module::defaultConfig("core");
    setDef("negatives", false);
}

void ComboCounter::settingsRender(float settingsOffset) {

    /* Border Start */

    initSettingsPage();

    addHeader("Combo Counter");
    defaultAddSettings("main");
    extraPadding();

    addHeader("Text");
    defaultAddSettings("text");
    extraPadding();

    addHeader("Colors");
    defaultAddSettings("colors");

    addHeader("Misc");
    addToggle("Count to Negatives", "Allows the count to keep going down", "negatives");
    defaultAddSettings("misc");
    

    FlarialGUI::UnsetScrollView();

    resetPadding();

}

void ComboCounter::onAttack(AttackEvent &event) {
    if (!this->isEnabled()) return;

    auto uhdhdrclock = std::chrono::high_resolution_clock::now();

    if (Combo < 0) {
        Combo = 1;
        last_hit = uhdhdrclock;
        return;
    }

    if (now - last_hit > std::chrono::milliseconds(480)) {
        Combo++;
        last_hit = uhdhdrclock;
    }
}

void ComboCounter::onTick(TickEvent &event) {
    if (!this->isEnabled()) return;
    if (!SDK::clientInstance->getLocalPlayer()) return;

    int currentHurtTime = reinterpret_cast<LocalPlayer*>(event.getActor())->getHurtTime();
    bool meow = getOps<bool>("negatives");

    // just in case player toggles negatives off
    if (!meow && Combo < 0) Combo = 0;

    if (currentHurtTime > 0 && lastHurtTime == 0) {
        if (Combo > 0) Combo = 0;
        if (meow) Combo--;
    }
    lastHurtTime = currentHurtTime;

    std::chrono::duration<double> duration = std::chrono::high_resolution_clock::now() - last_hit;
    if (duration.count() >= 15) Combo = 0;
}

void ComboCounter::onRender(RenderEvent &event) {
    if (this->isEnabled()) {
        auto comboStr = FlarialGUI::cached_to_string(Combo);
        this->normalRender(8, comboStr);
    }
}
