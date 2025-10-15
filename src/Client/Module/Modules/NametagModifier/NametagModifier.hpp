#pragma once

#include "../Module.hpp"
#include "Events/Game/PerspectiveEvent.hpp"
#include "Events/Render/DrawNameTagEvent.hpp"


class NametagModifier : public Module {

private:

    static inline std::vector<uint8_t> original;
    static inline uintptr_t address;
    static inline bool patched = false;

public:

    NametagModifier(): Module("Nametag", "Shows your nametag for you while\nin 3rd person mode.",
                IDR_NAMETAG_PNG, "", false, {"third person nametag", "3rd person nametag"}) {
        address = GET_SIG_ADDRESS("ThirdPersonNametag");

        original.resize(6);
        Memory::copyBytes((LPVOID) address, original.data(), 6);
    }

    void defaultConfig() override;

    void onEnable() override;

    void onDisable() override;

    static void patch();

    static void unpatch();

    void settingsRender(float settingsOffset) override;

    void onGetViewPerspective(PerspectiveEvent& event);

    void onDrawNameTag(DrawNameTagEvent& event);
};
