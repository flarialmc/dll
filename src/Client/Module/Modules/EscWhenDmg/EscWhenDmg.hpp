//
// Created by tonyv on 11/20/2025.
//

#ifndef FLARIAL_ESCWHENDMG_HPP
#define FLARIAL_ESCWHENDMG_HPP

class EscWhenDmg final : public Module {
public:
    EscWhenDmg() : Module("Esc When Damaged", "Automatically presses Esc key when you take damage", IDR_ESCWHENDMG_PNG, "", false, {"esc", "exit", "inventory", "damage"}) {}

    void onSetup() override;
    void onEnable() override;
    void onDisable() override;
    void settingsRender(float settingsOffset) override;
    void defaultConfig() override;
    void onPacketEvent(PacketEvent& event);

private:
    uintptr_t mAddr{};
};

#endif //FLARIAL_ESCWHENDMG_HPP