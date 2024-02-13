#include "HitboxListener.hpp"

#include "../../Manager.hpp"
#include "../../../../Utils/Render/DrawUtils.hpp"

void renderBox(Player* player) {
	Module* box = ModuleManager::getModule("Hitbox");

	D2D1_COLOR_F color2;
	if (box->settings.getSettingByName<bool>("color_rgb")->value) color2 = FlarialGUI::rgbColor;
	else color2 = FlarialGUI::HexToColorF(box->settings.getSettingByName<std::string>("color")->value);
	auto localPlayer = SDK::clientInstance->getLocalPlayer();
    float dist = localPlayer->getPosition()->dist(*player->getPosition());
    if(player == nullptr) return;
	// This may let through some entites
	if (player == localPlayer || !player || !player->isAlive() || !localPlayer->isValidTarget(player) || !HitboxListener::canSeeArrXD[player->getNametag()] || dist > 30)
		return;

    DrawUtils::addEntityBox(player, (float)fmax(0.5f, 1 / (float)fmax(1, localPlayer->getRenderPositionComponent()->renderPos.dist(player->getRenderPositionComponent()->renderPos))), color2);}

void HitboxListener::onRender(RenderEvent& event) {
	if (!this->module->settings.getSettingByName<bool>("enabled")->value)
		return;

	if (!SDK::clientInstance || !SDK::clientInstance->getLocalPlayer() || !SDK::clientInstance->mcgame->mouseGrabbed || !SDK::clientInstance->getLocalPlayer()->level)
		return;

	auto player = SDK::clientInstance->getLocalPlayer();

    if(player != nullptr) {
        for (const auto& ent: player->level->getRuntimeActorList()) {
            if (ent != nullptr && ent->isPlayer() && ent->hasCategory(ActorCategory::Player))
                renderBox((Player*)ent);
        }
    }
}
