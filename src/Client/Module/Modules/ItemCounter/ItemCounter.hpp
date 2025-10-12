#pragma once

#include "../Module.hpp"
#include "Events/Game/TickEvent.hpp"
#include "Events/Render/RenderEvent.hpp"

class ItemCounter : public Module {
private:

	int items = 0;
	std::vector<int> subIndexesToRender = {};
	std::unordered_map<std::string, int> itemCountMap = {};

public:

	ItemCounter() : Module("Item Counter", "Item Counter",
		IDR_SKULL_PNG, "") {

	};

	void onEnable() override;

	void onDisable() override;

	void onSetup() override;

	int getNextAvailableIndex();

	void multiDefaultConfig(int subIndexInt);

	void multiClearConfig(int subIndexInt);

	void defaultConfig() override;

	void multiSettingsRender(int subIndexInt);

	void settingsRender(float settingsOffset) override;

	void multiNormalRenderCore(int index, std::string &text, int subIndexInt, bool useCustom = false);

	void onTick(TickEvent& event);

	void onRender(RenderEvent& event);
};
