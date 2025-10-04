#pragma once

#include <queue>

#include "../Module.hpp"
#include "../../../../SDK/Client/Options/OptionsParser.hpp"
#include "Events/Game/ContainerScreenControllerTickEvent.hpp"
#include "Events/Game/ContainerSlotHoveredEvent.hpp"

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
		IDR_KEYBOARD_PNG, "", false, {"java inventory"}) {
		
	};

	void onEnable() override;

	void onDisable() override;

	void settingsRender(float settingsOffset) override;

	int isSlotKeybind(std::string key);

	void onMouseInput(MouseEvent& event);

	void onKey(KeyEvent& event);

	void onContainerSlotHovered(ContainerSlotHoveredEvent& event);;

	void onContainerTick(ContainerScreenControllerTickEvent& event);;

	bool canSwap(std::string_view collectionName);

	void clearQueue();
};