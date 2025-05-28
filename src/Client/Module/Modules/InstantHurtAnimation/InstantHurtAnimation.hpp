#pragma once

#include "../Module.hpp"
#include "../../../Hook/Hooks/Game/PacketHooks.hpp"


class InstantHurtAnimation : public Module {
private:
	std::unordered_map<int64_t, std::chrono::steady_clock::time_point> hitEntities;
public:
	InstantHurtAnimation() : Module("Insta Hurt Animation", "Hurt animation becomes ping independent, helps time hits.",
		IDR_COMBO_PNG, "") {
		Module::setup();
	};

	void onEnable() override {
		Listen(this, PacketEvent, &InstantHurtAnimation::onPacketReceive)
			Listen(this, AttackEvent, &InstantHurtAnimation::onAttack)
			Module::onEnable();
	}

	void onDisable() override {
		Deafen(this, PacketEvent, &InstantHurtAnimation::onPacketReceive)
			Deafen(this, AttackEvent, &InstantHurtAnimation::onAttack)
			Module::onDisable();
	}

	void defaultConfig() override {
		Module::defaultConfig();
		if (settings.getSettingByName<bool>("onlyWithArmor") == nullptr) settings.addSetting("onlyWithArmor", true);
		if (settings.getSettingByName<bool>("tryToExcludeTeam") == nullptr) settings.addSetting("tryToExcludeTeam", true);
	}

	void settingsRender(float settingsOffset) override {
		float x = Constraints::PercentageConstraint(0.019, "left");
		float y = Constraints::PercentageConstraint(0.10, "top");

		const float scrollviewWidth = Constraints::RelativeConstraint(0.12, "height", true);

		FlarialGUI::ScrollBar(x, y, 140, Constraints::SpacingConstraint(5.5, scrollviewWidth), 2);
		FlarialGUI::SetScrollView(x - settingsOffset, Constraints::PercentageConstraint(0.00, "top"),
			Constraints::RelativeConstraint(1.0, "width"),
			Constraints::RelativeConstraint(0.88f, "height"));

		this->addHeader("Settings");
		this->addToggle("Try to exclude team", "", settings.getSettingByName<bool>("tryToExcludeTeam")->value);
		this->addToggle("Only with armor", "", settings.getSettingByName<bool>("onlyWithArmor")->value);

		FlarialGUI::UnsetScrollView();

		this->resetPadding();
	}

	void onPacketReceive(PacketEvent& event) {
		auto player = SDK::clientInstance->getLocalPlayer();
		if (!SDK::clientInstance->getLocalPlayer()) return;
		if (event.getPacket()->getId() == MinecraftPacketIds::ActorEvent) {
			auto ee = (EntityEventPacket*)event.getPacket();
			if (ee->RuntimeID != player->getRuntimeIDComponent()->runtimeID) {
				if (ee->EventID == ActorEvent::Hurt) {
					// Cancel hurt anim packet, that was caused by our hit
					auto it = hitEntities.find(ee->RuntimeID);
					if (it != hitEntities.end()) {
						auto now = std::chrono::steady_clock::now();
						auto hitTime = it->second;
						auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(now - hitTime).count();
						if (duration <= 500) {
							event.cancel();
						}
						else {
							hitEntities.erase(it);
						}
					}
				}
			}
		}
	}

	void onAttack(AttackEvent& event) {
		if (!event.getActor()->isValid()) return;
		if (!event.getActor()->isValidAABB()) return;
		if (!event.getActor()->hasCategory(ActorCategory::Player)) return;
		if (this->settings.getSettingByName<bool>("onlyWithArmor")->value) {
			auto armorContainer = event.getActor()->getArmorContainer();
			if (armorContainer == nullptr) return;

			ItemStack* helmetItem = armorContainer->getItem(0);
			ItemStack* chestplateItem = armorContainer->getItem(1);
			ItemStack* leggingsItem = armorContainer->getItem(2);
			ItemStack* bootsItem = armorContainer->getItem(3);

			if (!helmetItem->item || !chestplateItem->item || !leggingsItem->item || !bootsItem->item) return;
		}
		if (this->settings.getSettingByName<bool>("tryToExcludeTeam")->value)
			if (event.getActor()->IsOnSameTeam(SDK::clientInstance->getLocalPlayer())) return;

		ClearOldHits();

		auto runtimeID = event.getActor()->getRuntimeIDComponent()->runtimeID;
		auto now = std::chrono::steady_clock::now();

		auto it = hitEntities.find(runtimeID);
		if (it != hitEntities.end()) {
			auto hitTime = it->second;
			auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(now - hitTime).count();
			if (duration <= 500) {
				return;
			}
		}

		std::shared_ptr<Packet> packet = SDK::createPacket((int)MinecraftPacketIds::ActorEvent);
		auto ee = (EntityEventPacket*)packet.get();
		ee->EventID = ActorEvent::Hurt;
		ee->RuntimeID = runtimeID;

		hitEntities[runtimeID] = now;

		if (SendPacketHook::PacketHandlerDispatcher && SendPacketHook::NetworkIdentifier && SendPacketHook::NetEventCallback) {
			SendPacketHook::receivePacketEntityEventOriginal(SendPacketHook::PacketHandlerDispatcher,
				SendPacketHook::NetworkIdentifier,
				SendPacketHook::NetEventCallback,
				packet);
		}
	};

	void ClearOldHits() {
		auto now = std::chrono::steady_clock::now();
		for (auto it = hitEntities.begin(); it != hitEntities.end(); ) {
			auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(now - it->second).count();
			if (duration > 500) {
				it = hitEntities.erase(it);
			}
			else {
				++it;
			}
		}
	}
};
