#pragma once

#include "../Module.hpp"
#include "Events/Render/RenderEvent.hpp"
#include "Events/Render/SetupAndRenderEvent.hpp"
#include "../../../../Assets/Assets.hpp"
#include "Events/Events.hpp"

class EntityCounter : public Module {
private:
	int entityCount = 0;
public:

	EntityCounter() : Module("Entity Counter", "Counts the entities in the surrounding area",
		IDR_ENTITYCOUNTER_PNG, "") {
		
	};

	void onEnable() override;

	void onDisable() override;

	void defaultConfig() override;

	void settingsRender(float settingsOffset) override;

	void onSetupAndRender(SetupAndRenderEvent& event);

	void onRender(RenderEvent& event);
};
