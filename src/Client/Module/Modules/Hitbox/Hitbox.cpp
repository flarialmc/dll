#include "Hitbox.hpp"

void Hitbox::onEnable() {
    Listen(this, SetupAndRenderEvent, &Hitbox::onSetupAndRender)
    Listen(this, RenderEvent, &Hitbox::onRender)
    Module::onEnable();
}

void Hitbox::onDisable() {
    Deafen(this, SetupAndRenderEvent, &Hitbox::onSetupAndRender)
    Deafen(this, RenderEvent, &Hitbox::onRender)
    Module::onDisable();
}

void Hitbox::defaultConfig() {
    settings.renameSetting("color", "colorOpacity", "color_rgb", "hitbox");
    Module::defaultConfig("core");
    setDef("hitbox", (std::string)"FFFFFF", 0.6f, false);
    setDef("thickness", 1.1f);
    setDef("staticThickness", false);
    setDef("outline", false);
    
}

void Hitbox::settingsRender(float settingsOffset) {

    initSettingsPage();

    addHeader("Hitbox");
    addToggle("2D Mode", "", "outline");
    addToggle("Static Thickness", "", "staticThickness");
    addSlider("Thickness", "", "thickness");
    extraPadding();

    addHeader("Colors");
    addColorPicker("Hitbox Color", "", "hitbox");

    FlarialGUI::UnsetScrollView();

    resetPadding();

}

void Hitbox::onSetupAndRender(SetupAndRenderEvent &event) {
    if (!this->isEnabled()) return;
    std::lock_guard<std::mutex> guard(renderMtx);
    aabbsToRender.clear();
    if (!SDK::clientInstance || !SDK::clientInstance->getLocalPlayer() || SDK::getCurrentScreen() != "hud_screen" ||
        !SDK::clientInstance->getLocalPlayer()->getLevel())
        return;
    auto player = SDK::clientInstance->getLocalPlayer();
    auto selectedEntity = player->getLevel()->getHitResult().getEntity();
    for (const auto& ent : player->getLevel()->getRuntimeActorList()) {
        auto pos = ent->getPosition();
        if (!pos) continue;
        if (ent != nullptr && ent != player && ent->hasCategory(ActorCategory::Mob)/*&& ent->isPlayer() && ent->hasCategory(ActorCategory::Player)*/) {
            float dist = player->getPosition()->dist(*pos);
            // This may let through some entites
            if (!ent->isValid() || !ent->isValidAABB() || dist > 30 || !player->canSee(*ent) ||
                ent->getActorFlag(ActorFlags::FLAG_INVISIBLE))
                continue;

            auto aabb = ent->getLerpedAABB();
            auto hitbox = ent->getLerpedAABB(true);

            AABBInfo info = { aabb, hitbox, selectedEntity == ent };

            aabbsToRender.emplace_back(info);
        }
    }
}

void Hitbox::onRender(RenderEvent &event) {
    if (!this->isEnabled()) return;
    if (!SDK::clientInstance || !SDK::clientInstance->getLocalPlayer() || SDK::getCurrentScreen() != "hud_screen" ||
        !SDK::clientInstance->getLocalPlayer()->getLevel())
        return;

    auto player = SDK::clientInstance->getLocalPlayer();

    if (FlarialGUI::inMenu || SDK::getCurrentScreen() != "hud_screen") return;

    if (player != nullptr) {
        std::lock_guard<std::mutex> guard(renderMtx);
        for (const auto& aabbInfo : aabbsToRender) {
            // Retrieve the thickness setting value from the module settings
            float thickness = getOps<float>("thickness");
            bool staticThickness = getOps<bool>("staticThickness");
            bool outline = getOps<bool>("outline");

            float lineWidth = thickness;

            if (!staticThickness) {
                // Get the render position of the player and compute the distance to the AABB lower bound
                float distance = player->getRenderPositionComponent()->renderPos.dist(aabbInfo.aabb.lower);

                // Compute the scaling factor based on the distance, ensuring it does not exceed the max distance of 30
                float scaleFactor = 1.f - (distance / 30.0f);

                // Calculate the line width, ensuring it does not fall below a minimum threshold of 0.5
                lineWidth = thickness * scaleFactor;
            }

            DrawUtils::addBox(aabbInfo.aabb.lower, aabbInfo.aabb.upper, staticThickness ? thickness : lineWidth, outline ? 2 : 1, getColor("hitbox"));

            //DrawUtils::addBox(aabbInfo.hitbox.lower, aabbInfo.hitbox.upper, staticThickness ? thickness : lineWidth, outline ? 2 : 1, aabbInfo.selected ? FlarialGUI::rgbColor :color2);
        }
    }
}
