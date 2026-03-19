#pragma once

#include <mutex>

#include "../Module.hpp"
#include "Events/Game/TickEvent.hpp"
#include "Events/Render/RenderEvent.hpp"
#include "Events/Render/SetupAndRenderEvent.hpp"

class ItemCounter : public Module {
private:
	struct IconRenderRequest {
		int subIndex = 0;
		float screenX = 0.0f;
		float screenY = 0.0f;
		float scale = 1.0f;
		float iconSize = 0.0f;
		int itemCount = 0;
		bool drawBottomRightCount = false;
		bool useMinecraftCountFont = false;
		float vanillaShadowOffset = 0.0f; // shadow offset in D2D screen pixels
		float vanillaFontScale = 1.0f;    // MC font scale (textscale * uiscale-proportion)
	};

	int getNextAvailableOrder();
	std::vector<int> getOrderedSubIndexes();
	void normalizeDisplayOrder();

	int items = 0;
	std::vector<int> subIndexesToRender = {};
	std::unordered_map<int, int> itemCountBySubIndex = {};
	std::unordered_map<int, bool> itemSeenInInventory = {};
	std::unordered_map<int, std::string> resolvedItemIdBySubIndex = {};
	std::vector<IconRenderRequest> iconRenderQueue = {};
	Vec2<float> lastGroupBounds = Vec2(0.0f, 0.0f);

	// Registry validation cache: maps (subIndex, normalizedItemName) -> exists in registry.
	// Populated lazily during onTick; cleared when setup runs.
	std::unordered_map<std::string, bool> registryValidationCache = {};
	bool registryAvailable = false;
	bool tickDataReady = false;

	// Protects shared maps (itemCountBySubIndex, itemSeenInInventory, resolvedItemIdBySubIndex,
	// registryValidationCache, registryAvailable, tickDataReady) from concurrent tick/render access.
	// onTick locks when writing; onRender/settingsRender snapshot under lock then read locally.
	std::mutex tickDataMutex;

public:

	ItemCounter() : Module("Item Counter", "Item Counter",
		IDR_MULTIPLIER_PNG, "") {

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

	void multiNormalRenderCore(
		int index,
		std::string &text,
		int subIndexInt,
		bool useCustom = false,
		bool showTexture = false,
		bool drawCountBottomRight = false,
		int itemCount = 0,
		bool useMinecraftCountFont = false,
		const Vec2<float>* forcedTopLeft = nullptr,
		Vec2<float>* outRectSize = nullptr,
		bool suppressText = false
	);

	void onTick(TickEvent& event);

	void onRender(RenderEvent& event);

	void onSetupAndRender(SetupAndRenderEvent& event);
};
