#include "HitboxListener.hpp"
#include "../../../../Utils/Render/DrawUtils.hpp"

void HitboxListener::onSetupAndRender(SetupAndRenderEvent &event) {
    entitiesToRender.clear();
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

            // Add to render list
            entitiesToRender.insert(ent->getEntityId());
        }
    }
}

void HitboxListener::onRender(RenderEvent &event) {
    if (!this->module->isEnabled())
        return;

    if (!SDK::clientInstance || !SDK::clientInstance->getLocalPlayer() || !SDK::clientInstance->mcgame->mouseGrabbed ||
        !SDK::clientInstance->getLocalPlayer()->level)
        return;

    auto player = SDK::clientInstance->getLocalPlayer();

    if (player != nullptr) {
        D2D1_COLOR_F color2;
        // TODO: optimize getting colors
        if (module->settings.getSettingByName<bool>("color_rgb")->value) color2 = FlarialGUI::rgbColor;
        else color2 = FlarialGUI::HexToColorF(module->settings.getSettingByName<std::string>("color")->value);

        for (const auto &ent: player->level->getRuntimeActorList()) {
            if (entitiesToRender.contains(ent->getEntityId())){
                DrawUtils::addEntityBox(ent, (float)fmax(0.5f, 1 / (float)fmax(1,
                                                                               player->getRenderPositionComponent()->renderPos.dist(
                                                                                       ent->getRenderPositionComponent()->renderPos))),
                                        color2);
            }
        }
    }
}
