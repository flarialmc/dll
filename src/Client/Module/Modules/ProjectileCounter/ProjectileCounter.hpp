#pragma once
#include "Module.hpp"

class ProjectileCounter : public Module {
public:
    ProjectileCounter() : Module("Projectile Counter", "Shows how many projectiles you have.", Category::Visual) {}

    void onEnable() override;
    void onDisable() override;
    void onRender(RenderEvent& event);
    void settingsRender(float settingsOffset) override;
    void defaultConfig() override;

private:
    int getItemCount(std::string itemName);
};
