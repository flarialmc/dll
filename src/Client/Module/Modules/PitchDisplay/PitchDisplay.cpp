#include "PitchDisplay.hpp"

void PitchDisplay::customConfig()
{
	setDef("onlyRenderWhenElytraEquipped", false);
}

void PitchDisplay::customSettings()
{
	addToggle("Only render when elytra is equipped", "", "onlyRenderWhenElytraEquipped");
}

std::string PitchDisplay::getDisplayValue()
{
	if (SDK::clientInstance == nullptr) return "";
	LocalPlayer *player = SDK::clientInstance->getLocalPlayer();
	if (!player) return "";

	bool shouldRender = true;

	if (getOps<bool>("onlyRenderWhenElytraEquipped")) {
		shouldRender = false;
		if (player->getSupplies() != nullptr) {
			if (auto *chestArmor = player->getArmor(1); chestArmor != nullptr) {
				if (auto *item = chestArmor->getItem(); item != nullptr && item->name == "elytra") {
					shouldRender = true;
				}
			}
		}
	}

	if (!shouldRender) return "";

	ActorRotationComponent *rotComponent = player->getActorRotationComponent();
	if (!rotComponent) return "";

	if (rotComponent->rot.x != 0) pitch = rotComponent->rot.x;

	return std::format("{:.1f}", pitch);
}
