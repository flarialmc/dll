#pragma once

#include "../Module.hpp"
#include "../../../Events/Game/RenderItemInHandEvent.hpp"
#include "Events/Game/PerspectiveEvent.hpp"
#include "Events/Game/SwingDurationEvent.hpp"
#include "../../../../Assets/Assets.hpp"

class SwingAnimations : public Module {
public:
	Perspective perspective;
	static inline uintptr_t fluxSwing;
	static inline std::array<std::byte, 5> original;
	bool previousFluxSwingState = false;
	bool isPatched = false;

	SwingAnimations() : Module("Swing Animations", "Customizable swing animations",
		IDR_SWORD_PNG, "", false, {"swing", "animations", "sword"})
	{
		fluxSwing = GET_SIG_ADDRESS("FluxSwing");
		original = std::bit_cast<std::array<std::byte, 5>>(*(std::byte(*)[5]) fluxSwing);
	};

	void onEnable() override;

	void onDisable() override;

	void onPerspectiveChange(PerspectiveEvent& event);

	void onItemInHandRender(RenderItemInHandEvent& event);

	void onSwingDuration(SwingDurationEvent& event);

	void defaultConfig() override;

	void settingsRender(float settingsOffset) override;

	void patch()
	{
		if (fluxSwing == 0) { Logger::error("addr invalid?"); return; }
		Memory::nopBytes((void*)fluxSwing, 5);
	}

	void unpatch()
	{
		if (fluxSwing == 0) { Logger::error("addr invalid?"); return; }
		Memory::patchBytes((void*)fluxSwing, original.data(), 5);
	}
};