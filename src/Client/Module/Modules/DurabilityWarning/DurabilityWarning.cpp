#include "DurabilityWarning.hpp"
#include "GUI/Engine/EngineImGui.hpp"

void DurabilityWarning::defaultConfig() {
	Module::defaultConfig("core");
	setDef("threshold", 10.f);
}

void DurabilityWarning::settingsRender(float settingsOffset) {
	initSettingsPage();

	addSlider("Durability Threshold", "Warn when remaining durability falls at or below this value.", "threshold", 100.f, 1.f);
	FlarialGUI::UnsetScrollView();

	resetPadding();
}

void DurabilityWarning::onEnable() {
	Module::onEnable();
	Listen(this, TickEvent, &DurabilityWarning::onTick);
}

void DurabilityWarning::onDisable() {
	Module::onDisable();
	Deafen(this, TickEvent, &DurabilityWarning::onTick);
	notified = false;
}

void DurabilityWarning::onTick(TickEvent& event) {
	if (!this->isEnabled()) return;
	if (!SDK::clientInstance) return;

	const auto player = SDK::clientInstance->getLocalPlayer();
	if (!player) return;

	const auto supplies = player->getSupplies();
	if (!supplies) return;

	const auto inventory = supplies->getInventory();
	if (!inventory) return;

	const int selectedSlot = supplies->getSelectedSlot();
	const auto item = inventory->getItem(selectedSlot);

	// If held item is not a damageable tool, reset and bail
	if (!item || !item->isValid() || item->getMaxDamage() <= 0) {
		notified = false;
		return;
	}

	const short remaining = item->getMaxDamage() - item->getDamageValue();
	const int threshold = static_cast<int>(getOps<float>("threshold"));

	if (remaining <= threshold) {
		if (!notified) {
			notified = true;
			auto* itemDef = item->getItem();
			std::string itemName = itemDef ? itemDef->name : "Your item";
			FlarialGUI::Notify(std::format("{} is at {} durability!", itemName, remaining));
		}
	} else {
		// Durability recovered above threshold (repaired, etc.) — allow re-notification
		notified = false;
	}
}
