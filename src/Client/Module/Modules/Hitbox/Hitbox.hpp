#pragma once

#include "../Module.hpp"
#include "../../../Client.hpp"
#include "../../../../Utils/Render/DrawUtils.hpp"
#include "Assets/Assets.hpp"

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

	Hitbox() : Module("Hitbox", "Displays hitboxes of entities",
		IDR_BLOCK_PNG, "") {

	};

	// TODO: add options to see all entities

	void onEnable() override;

	void onDisable() override;

	void defaultConfig() override;

	void settingsRender(float settingsOffset) override;

	void onSetupAndRender(SetupAndRenderEvent& event);

	void onRender(RenderEvent& event);
};
