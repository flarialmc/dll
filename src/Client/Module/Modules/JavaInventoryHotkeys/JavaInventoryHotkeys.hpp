#pragma once

#include "../Module.hpp"
#include "../../../../SDK/Client/Options/OptionsParser.hpp"

struct MoveRequest {
	int64_t destSlot;
	int64_t hoveredSlot;
	std::string collectionName;
};


class JavaInventoryHotkeys : public Module {
private:
	OptionsParser parser;
	int64_t currentHoveredSlot;
	std::string currentCollectionName;

	std::queue<MoveRequest> moveRequests;
	void* lastContainer = nullptr;
public:
	JavaInventoryHotkeys() : Module("Java Hotkeys",
		"Allows to swap items in your inventory java-like.",
		IDR_KEYBOARD_PNG, "") {
		Module::setup();
	};

	void onEnable() override {
		parser.parseOptionsFile();

		Listen(this, ContainerSlotHoveredEvent, &JavaInventoryHotkeys::onContainerSlotHovered)
			Listen(this, ContainerScreenControllerTickEvent, &JavaInventoryHotkeys::onContainerTick)
			Listen(this, MouseEvent, &JavaInventoryHotkeys::onMouseInput)
			Listen(this, KeyEvent, &JavaInventoryHotkeys::onKey)

			Module::onEnable();
	}

	void onDisable() override {
		Deafen(this, ContainerSlotHoveredEvent, &JavaInventoryHotkeys::onContainerSlotHovered)
			Deafen(this, ContainerScreenControllerTickEvent, &JavaInventoryHotkeys::onContainerTick)
			Deafen(this, MouseEvent, &JavaInventoryHotkeys::onMouseInput)
			Deafen(this, KeyEvent, &JavaInventoryHotkeys::onKey)
			Module::onDisable();
	}

	void settingsRender(float settingsOffset) override {
		float x = Constraints::PercentageConstraint(0.019, "left");
		float y = Constraints::PercentageConstraint(0.10, "top");

		const float scrollviewWidth = Constraints::RelativeConstraint(0.12, "height", true);


		FlarialGUI::ScrollBar(x, y, 140, Constraints::SpacingConstraint(5.5, scrollviewWidth), 2);
		FlarialGUI::SetScrollView(x - settingsOffset, Constraints::PercentageConstraint(0.00, "top"),
			Constraints::RelativeConstraint(1.0, "width"),
			Constraints::RelativeConstraint(0.88f, "height"));


		FlarialGUI::UnsetScrollView();

		this->resetPadding();
	}

	int isSlotKeybind(std::string key) {
		for (int i = 0; i < 9; i++) {
			std::string option = parser.options["ctrl_fullkeyboardgameplay"] == "1" ? "keyboard_type_1_key.hotbar." : "keyboard_type_0_key.hotbar.";
			auto value = parser.options[option + FlarialGUI::cached_to_string(i + 1)];
			if (value == key) return i;
		}

		return -1;
	}

	void onMouseInput(MouseEvent& event) {
		if (event.getAction() != MouseAction::Press) return;
		std::string screen = SDK::getCurrentScreen();
		if (screen == "hud_screen" || screen == "pause_screen") return;

		auto targetSlot = isSlotKeybind(FlarialGUI::cached_to_string(-100 + (int)event.getButton()));
		if (targetSlot == -1) return;

		moveRequests.push({ targetSlot, currentHoveredSlot, currentCollectionName });
	}

	void onKey(KeyEvent& event) {
		std::string screen = SDK::getCurrentScreen();
		if (screen == "hud_screen" || screen == "pause_screen") {
			clearQueue();
		}
		if (screen != "inventory_screen") return;
		if (event.getAction() != ActionType::Pressed) return;

		auto targetSlot = isSlotKeybind(FlarialGUI::cached_to_string(event.getKey()));
		if (targetSlot == -1) return;

		moveRequests.push({ targetSlot, currentHoveredSlot, currentCollectionName });
	}

	void onContainerSlotHovered(ContainerSlotHoveredEvent& event) {
		currentHoveredSlot = event.getHoveredSlot();
		currentCollectionName = event.getCollectionName();
	};

	void onContainerTick(ContainerScreenControllerTickEvent& event) {
		auto controller = event.getContainerScreenController();
		if (!controller) return;
		if (lastContainer != controller) {
			clearQueue();
			lastContainer = controller;
		};

		for (moveRequests; !moveRequests.empty(); moveRequests.pop()) {
			auto request = moveRequests.front();

			if (!canSwap(request.collectionName)) continue;
			controller->swap(request.collectionName, request.hoveredSlot, "hotbar_items", request.destSlot);
		}
	};

	bool canSwap(std::string_view collectionName) {
		bool isItemContainer = collectionName.find("_item") != std::string_view::npos;
		bool isRecipeContainer = collectionName.find("recipe_") != std::string_view::npos;
		bool isSearchContainerOrBar = collectionName.find("search") != std::string_view::npos;
		return isItemContainer || (isRecipeContainer && !isSearchContainerOrBar);
	}

	void clearQueue() {
		for (moveRequests; !moveRequests.empty(); moveRequests.pop()) {}
	}
};