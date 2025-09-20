#pragma once

#include "../Module.hpp"
#include "../../../Events/Game/RenderItemInHandEvent.hpp"
#include "Events/Game/PerspectiveEvent.hpp"
#include "Events/Game/SwingDurationEvent.hpp"
#include "../../../../Assets/Assets.hpp"
#include "Modules/Doom/Doom.hpp"

class SwingAnimations : public Module {
public:
	Perspective perspective;
	static inline uintptr_t fluxSwing;
	static inline std::array<std::byte, 5> original;
	bool previousFluxSwingState = false;
	bool isPatched = false;

	static inline uintptr_t swingAngleAddr = 0;
	static inline float* swingAnglePtr = nullptr;
	static inline float originalSwingAngle = 0.0f;
	float previousSwingAngle = -80.0f;
	bool swingAngleInitialized = false;

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

	void initializeSwingAngle()
	{
		if (swingAngleInitialized) return;

		auto addr = GET_SIG_ADDRESS("SwingAngle");
		if (!addr) {
			Logger::error("SwingAngle signature not found");
			swingAngleAddr = 0;
			swingAnglePtr = nullptr;
			return;
		}

		try {
			// Calculate the absolute address using the offset and relative address
			uintptr_t offset = GET_OFFSET("SwingAngle");
			if (offset == 0) {
				Logger::error("SwingAngle offset is 0");
				return;
			}

			// Get the 4-byte relative offset at addr + offset
			int32_t relativeOffset = *reinterpret_cast<int32_t*>(addr + offset);

			// Calculate absolute address: instruction_after_offset + relative_offset
			uintptr_t absolute = (addr + offset + 4) + static_cast<int64_t>(relativeOffset);

			Logger::debug("SwingAngle addr: 0x{:X}, offset: {}, relative: 0x{:X}, absolute: 0x{:X}",
						  addr, offset, relativeOffset, absolute);

			swingAngleAddr = absolute;
			swingAnglePtr = reinterpret_cast<float*>(swingAngleAddr);

			// Validate the pointer before accessing it
			if (!swingAnglePtr || IsBadReadPtr(swingAnglePtr, sizeof(float))) {
				Logger::error("Invalid SwingAngle pointer: 0x{:X}", reinterpret_cast<uintptr_t>(swingAnglePtr));
				swingAnglePtr = nullptr;
				return;
			}

			Memory::SetProtection(swingAngleAddr, sizeof(float), PAGE_EXECUTE_READWRITE);
			originalSwingAngle = *swingAnglePtr;
			swingAngleInitialized = true;

			Logger::info("SwingAngle initialized successfully. Original value: {}", originalSwingAngle);
		}
		catch (const std::exception& e) {
			Logger::error("Exception in initializeSwingAngle: {}", e.what());
			swingAnglePtr = nullptr;
		}
		catch (...) {
			Logger::error("Unknown exception in initializeSwingAngle");
			swingAnglePtr = nullptr;
		}
	}
	double degreesToRadians(double degrees) {
		return degrees * (M_PI / 180.0);
	}
	void updateSwingAngle(float newAngle)
	{
		if (!swingAngleInitialized) initializeSwingAngle();
		if (!swingAnglePtr || !swingAngleInitialized) return;

		try {
			if (!IsBadWritePtr(swingAnglePtr, sizeof(float))) {
				*swingAnglePtr = degreesToRadians(newAngle);
			} else {
				Logger::error("Cannot write to SwingAngle pointer");
			}
		}
		catch (...) {
			Logger::error("Exception while updating swing angle");
		}
	}

	void restoreSwingAngle()
	{
		if (!swingAngleInitialized || !swingAnglePtr) return;

		try {
			if (!IsBadWritePtr(swingAnglePtr, sizeof(float))) {
				*swingAnglePtr = originalSwingAngle;
			} else {
				Logger::error("Cannot write to SwingAngle pointer during restore");
			}
		}
		catch (...) {
			Logger::error("Exception while restoring swing angle");
		}
	}
};
