#include "HitboxListener.hpp"

#include "../../Manager.hpp"
#include "../../../../Utils/Render/DrawUtils.hpp"

void renderBox(Player* player) {
	Module* box = ModuleManager::getModule("Hitbox");

	D2D1_COLOR_F color2;
	if (box->settings.getSettingByName<bool>("color_rgb")->value) color2 = FlarialGUI::rgbColor;
	else color2 = FlarialGUI::HexToColorF(box->settings.getSettingByName<std::string>("color")->value);

	MCCColor c;
	c.r = color2.r;
	c.g = color2.g;
	c.b = color2.b;
	c.a = box->settings.getSettingByName<float>("colorOpacity")->value;

	auto localPlayer = SDK::clientInstance->getLocalPlayer();

	// This may let through some entites
	if (player == localPlayer || player == nullptr || !player->isAlive() || !localPlayer->canSee(player) || !localPlayer->isValidTarget(player))
		return;

	DrawUtils::addEntityBox(player, (float)fmax(0.5f, 1 / (float)fmax(1, localPlayer->getRenderPositionComponent()->renderPos.dist(player->getRenderPositionComponent()->renderPos))), c);
}

void HitboxListener::onSetupAndRender(SetupAndRenderEvent& event) {
	if (!this->module->settings.getSettingByName<bool>("enabled")->value)
		return;

	if (!SDK::clientInstance || !SDK::clientInstance->getLocalPlayer() || !SDK::clientInstance->mcgame->mouseGrabbed || !SDK::clientInstance->getLocalPlayer()->level)
		return;

	auto player = event.muirc->clientInstance->getLocalPlayer();

	DrawUtils::setColor(1.f, 1.f, 1.f, 1.f);

	auto tess = DrawUtils::getTessellator();

	tess->begin(VertextFormat::TRIANGLE_LIST);

	for (const auto& ent : player->level->getRuntimeActorList()) {
		if (ent != nullptr && ent->isPlayer() && ent->hasCategory(ActorCategory::Player))
			renderBox(reinterpret_cast<Player*>(ent));
	}

	tess->renderMeshImmediately(DrawUtils::getScreenContext(), DrawUtils::getUIMaterial());
}
