#include "HitboxListener.hpp"
#include "../../../../Utils/Render/DrawUtils.hpp"
#include "../../../../SDK/Client/Actor/Components/ActorOwnerComponent.hpp"

void HitboxListener::onSetupAndRender(SetupAndRenderEvent &event) {
    // if(WinrtUtils::checkAboveOrEqual(21,40)) return HitboxListener::fillDataToRender();
    std::lock_guard<std::mutex> guard(renderMtx);
    aabbsToRender.clear();
    if (!SDK::clientInstance || !SDK::clientInstance->getLocalPlayer() || SDK::getCurrentScreen() != "hud_screen" ||
        !SDK::clientInstance->getLocalPlayer()->getLevel())
        return;
    auto player = SDK::clientInstance->getLocalPlayer();
    for (const auto &ent: player->getLevel()->getRuntimeActorList()) {
        auto pos = ent->getPosition();
        if(!pos) continue;
        if (ent != nullptr && ent != player /*&& ent->isPlayer() && ent->hasCategory(ActorCategory::Player)*/) {
            float dist = player->getPosition()->dist(*pos);
            // This may let through some entites
            if (!ent->isValidAABB() || dist > 30 || !player->canSee(*ent) ||
                ent->getActorFlag(ActorFlags::FLAG_INVISIBLE))
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

    if (!SDK::clientInstance || !SDK::clientInstance->getLocalPlayer() || SDK::getCurrentScreen() != "hud_screen" ||
        !SDK::clientInstance->getLocalPlayer()->getLevel())
        return;

    auto player = SDK::clientInstance->getLocalPlayer();

    if(FlarialGUI::inMenu || SDK::getCurrentScreen() != "hud_screen") return;

    if (player != nullptr) {
        D2D1_COLOR_F color2;
        // TODO: optimize getting colors
        if (module->settings.getSettingByName<bool>("color_rgb")->value) color2 = FlarialGUI::rgbColor;
        else color2 = FlarialGUI::HexToColorF(module->settings.getSettingByName<std::string>("color")->value);

        std::lock_guard<std::mutex> guard(renderMtx);
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

void HitboxListener::fillDataToRender() {
    std::lock_guard<std::mutex> guard(renderMtx);
    aabbsToRender.clear();
    if (!SDK::clientInstance || !SDK::clientInstance->getLocalPlayer() || SDK::getCurrentScreen() != "hud_screen" ||
        !SDK::clientInstance->getLocalPlayer()->getLevel())
        return;
    auto player = SDK::clientInstance->getLocalPlayer();
    auto ctx = player->GetEntityContextV1_20_50();

    if(!ctx) return;

    auto view = ctx->enttRegistry->view<RenderPositionComponent, AABBShapeComponent, ActorOwnerComponent>();
    for (auto entity : view) {
        if(!ctx->hasComponent<RenderPositionComponent>(entity)) continue;
        if(!ctx->hasComponent<AABBShapeComponent>(entity)) continue;
        if(!ctx->hasComponent<ActorOwnerComponent>(entity)) continue;
        auto [renderPos, aabb, owner] = view.operator[](entity);
        static Vec3<float> zeroPos = Vec3<float>{0.f, 0.f, 0.f};
        if(renderPos.renderPos == zeroPos) continue;
        if (owner.actor != player /*&& ent->isPlayer() && ent->hasCategory(ActorCategory::Player)*/) {
            float dist = player->getPosition()->dist(renderPos.renderPos);
            // This may let through some entites
            if (dist > 30 || !owner.actor->isValidAABB() || !player->canSee(*owner.actor) ||
                    owner.actor->getActorFlag(ActorFlags::FLAG_INVISIBLE))
                continue;

            float mod = 0.f;

            if (owner.actor->hasCategory(ActorCategory::Player))
                mod = 1.6f;

            auto& aabbSize = aabb.size;
            auto lower = renderPos.renderPos.sub(aabbSize.x / 2.f, mod, aabbSize.x / 2.f), upper = lower.add(aabbSize.x, aabbSize.y, aabbSize.x);

            aabbsToRender.emplace_back(lower, upper);
        }
    }
}
