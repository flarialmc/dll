#pragma once

#include "../Module.hpp"
#include "../../../Client.hpp"
#include "../../../../Utils/Render/DrawUtils.hpp"

struct AABBInfo {
	AABB aabb;
	AABB hitbox;
	bool selected;
};

class Hitbox : public Module {
private:
	static inline std::vector<AABBInfo> aabbsToRender;
	static inline std::mutex renderMtx;
public:

	Hitbox() : Module("Hitbox", "Displays hitboxes of entities", IDR_BLOCK_PNG, "") {

		Module::setup();

	};

	// TODO: add options to see all entities

	void onEnable() override {
		Listen(this, SetupAndRenderEvent, &Hitbox::onSetupAndRender)
			Listen(this, RenderEvent, &Hitbox::onRender)
			Module::onEnable();
	}

	void onDisable() override {
		Deafen(this, SetupAndRenderEvent, &Hitbox::onSetupAndRender)
			Deafen(this, RenderEvent, &Hitbox::onRender)
			Module::onDisable();
	}

	void defaultConfig() override {
		Module::defaultConfig("core");
		setDef("hitbox", (std::string)"FFFFFF", 0.6f, false);
		setDef("thickness", 1.1f);
		setDef("staticThickness", false);
		setDef("outline", false);
	}

	void settingsRender(float settingsOffset) override {

		float x = Constraints::PercentageConstraint(0.019, "left");
		float y = Constraints::PercentageConstraint(0.10, "top");

		const float scrollviewWidth = Constraints::RelativeConstraint(0.12, "height", true);


		FlarialGUI::ScrollBar(x, y, 140, Constraints::SpacingConstraint(5.5, scrollviewWidth), 2);
		FlarialGUI::SetScrollView(x - settingsOffset, Constraints::PercentageConstraint(0.00, "top"),
			Constraints::RelativeConstraint(1.0, "width"),
			Constraints::RelativeConstraint(0.88f, "height"));

		addHeader("Hitbox");
		addToggle("2D Mode", "", getOps<bool>("outline"));
		addToggle("Static Thickness", "", getOps<bool>("staticThickness"));
		addSlider("Thickness", "", getOps<float>("thickness"));
		extraPadding();

		addHeader("Colors");
		addColorPicker("Hitbox Color", "", "hitbox");

		FlarialGUI::UnsetScrollView();

		resetPadding();

	}

	void onSetupAndRender(SetupAndRenderEvent& event) {
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

	void onRender(RenderEvent& event) {

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
};
