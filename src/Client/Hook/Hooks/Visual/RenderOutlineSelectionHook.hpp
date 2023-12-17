#pragma once

#include "../Hook.hpp"
#include "../../../../Utils/Memory/Memory.hpp"
#include "../../../../Utils/Utils.hpp"
#include "../../../../Utils/Memory/Memory.hpp"
#include "../../../Module/Manager.hpp"
#include "../../../../SDK/SDK.hpp"
#include "../../../../SDK/Client/Render/ScreenContext.hpp"

class RenderOutlineSelectionHook : public Hook
{
private:

    static void RenderOutlineSelectionCallback(LevelRendererPlayer* _this, ScreenContext& screenContext, Block const& block, BlockSource& region, Vec3<int> const& pos) {

        auto mod = ModuleManager::getModule("BlockOutline");
        bool enabled = mod ? mod->settings.getSettingByName<bool>("enabled")->value : false;

        static auto color = Memory::offsetFromSig(Memory::findSig("0F 10 05 ? ? ? ? 0F 11 00 C6 40 10 ? 0F 57 C9 0F 11 4D"), 3);

        if (enabled) {
            D2D1_COLOR_F color2;
            if (mod->settings.getSettingByName<bool>("color_rgb")->value) color2 = FlarialGUI::rgbColor;
            else color2 = FlarialGUI::HexToColorF(mod->settings.getSettingByName<std::string>("color")->value);

            MCCColor c;
            c.r = color2.r;
            c.g = color2.g;
            c.b = color2.b;
            c.a = mod->settings.getSettingByName<float>("colorOpacity")->value;

            Memory::patchBytes((void*)color, &c, 0x10);
        }

        func_original(_this, screenContext, block, region, pos);

        if (enabled) {
            static MCCColor black(0.f, 0.f, 0.f, 1.f);

            Memory::patchBytes((void*)color, &black, 0x10);
        }

    }

public:
    typedef void (__thiscall* RenderOutlineSelectionOriginal)(LevelRendererPlayer*, ScreenContext&, Block const&, BlockSource&, Vec3<int> const&);
    static inline RenderOutlineSelectionOriginal func_original = nullptr;

    RenderOutlineSelectionHook() : Hook("renderOutlineSelection Hook", "48 8B C4 55 53 56 57 41 56 41 57 48 8D 68 ? 48 81 EC ? ? ? ? 0F 29 70 ? 0F 29 78 ? 44 0F 29 ? ? 44 0F 29 ? ? 48 8B 05") {}

    void enableHook() override {

        this->autoHook(RenderOutlineSelectionCallback, (void**)&func_original);

    }
};


