#include "HitboxListener.hpp"
#include "../../../../Utils/Render/DrawUtils.hpp"

void HitboxListener::onSetupAndRender(SetupAndRenderEvent &event) {
    aabbsToRender.clear();
    if (!SDK::clientInstance || !SDK::clientInstance->getLocalPlayer() || !SDK::clientInstance->mcgame->mouseGrabbed ||
        !SDK::clientInstance->getLocalPlayer()->level)
        return;
    auto player = SDK::clientInstance->getLocalPlayer();
    for (const auto &ent: player->level->getRuntimeActorList()) {
        if (ent != nullptr && ent != player /*&& ent->isPlayer() && ent->hasCategory(ActorCategory::Player)*/) {
            float dist = player->getPosition()->dist(*ent->getPosition());
            // This may let through some entites
            if (!ent->isAlive() || !player->isValidTarget(ent) || dist > 30 || !player->canSee(*ent) ||
                ent->getActorFlag(ActorFlags::FLAG_INVISIBLE)) // + ent == player ||
                continue;

            float mod = 0.f;

            if (ent->hasCategory(ActorCategory::Player))
                mod = 1.6f;

            auto& aabbSize = ent->getAABBShapeComponent()->size;
            auto& renderPos = ent->getRenderPositionComponent()->renderPos;
            auto lower = renderPos.sub(aabbSize.x / 2.f, mod, aabbSize.x / 2.f), upper = lower.add(aabbSize.x, aabbSize.y, aabbSize.x);

            aabbsToRender.emplace_back(lower, upper);
        }
    }
}

void HitboxListener::onRender(RenderEvent &event) {

    if (!SDK::clientInstance || !SDK::clientInstance->getLocalPlayer() || !SDK::clientInstance->mcgame->mouseGrabbed ||
        !SDK::clientInstance->getLocalPlayer()->level)
        return;

    auto player = SDK::clientInstance->getLocalPlayer();

    if (player != nullptr) {
        D2D1_COLOR_F color2;
        // TODO: optimize getting colors
        if (module->settings.getSettingByName<bool>("color_rgb")->value) color2 = FlarialGUI::rgbColor;
        else color2 = FlarialGUI::HexToColorF(module->settings.getSettingByName<std::string>("color")->value);

        for (const auto &aabb: aabbsToRender) {
            // Retrieve the thickness setting value from the module settings
            float thickness = module->settings.getSettingByName<float>("thickness")->value;
            bool staticThickness = module->settings.getSettingByName<bool>("staticThickness")->value;
            bool outline = module->settings.getSettingByName<bool>("outline")->value;

            float lineWidth = thickness;

            if(!staticThickness) {
                // Get the render position of the player and compute the distance to the AABB lower bound
                float distance = player->getRenderPositionComponent()->renderPos.dist(aabb.lower);

                // Compute the scaling factor based on the distance, ensuring it does not exceed the max distance of 30
                float scaleFactor = 1.f - (distance / 30.0f);

                // Calculate the line width, ensuring it does not fall below a minimum threshold of 0.5
                lineWidth = thickness * scaleFactor;
            }

            DrawUtils::addBox(aabb.lower, aabb.upper, staticThickness ? thickness : lineWidth, outline ? 2 : 1, color2);
        }
    }
}
