#include "ComboCounter.hpp"

std::string ComboCounter::getDisplayValue() {
    return FlarialGUI::cached_to_string(Combo);
}

void ComboCounter::customConfig() {
    setDef("text", (std::string)"Combo: {value}");
    setDef("negatives", false);
}

void ComboCounter::customSettings() {
    addToggle("Count to Negatives", "Allows the count to keep going down", "negatives");
}

void ComboCounter::customInit() {
    Listen(this, AttackEvent, &ComboCounter::onAttack)
    Listen(this, TickEvent, &ComboCounter::onTick)
}

void ComboCounter::customCleanup() {
    Deafen(this, AttackEvent, &ComboCounter::onAttack)
    Deafen(this, TickEvent, &ComboCounter::onTick)
}

void ComboCounter::onAttack(AttackEvent &event) {
    if (!this->isEnabled()) return;

    auto now = std::chrono::high_resolution_clock::now();

    if (Combo < 0) {
        Combo = 1;
        last_hit = now;
        return;
    }

    if (now - last_hit > std::chrono::milliseconds(480)) {
        Combo++;
        last_hit = now;
    }
}

void ComboCounter::onTick(TickEvent &event) {
    if (!this->isEnabled()) return;

    if (!SDK::clientInstance || !SDK::clientInstance->getLocalPlayer()) return;

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
