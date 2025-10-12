#pragma once

#include "../Module.hpp"
#include "../../../Hook/Hooks/Game/ReadFileHook.hpp"
#include "src/Client/Hook/Hooks/Game/composeFullStack.hpp"

class MaterialBinLoader : public Module {
private:
    bool ispatched;

public:
    MaterialBinLoader() : Module("MaterialBinLoader", "Load Shaders from ResourcePack",
                      IDR_RENDER_DRAGON_PNG, "", false, {"shader", "shader loader", "render", "brd", "better render dragon", "betterrenderdragon", "matject"}) {
        Module::setup();
};

    void onEnable() override {
        Listen(this, ReadFileEvent, &MaterialBinLoader::onReadFile)
        Module::onEnable();
    }

    void onDisable() override {
        Deafen(this, ReadFileEvent, &MaterialBinLoader::onReadFile)
        Module::onDisable();
    }

    void defaultConfig() override { Module::defaultConfig();
    }

    void settingsRender(float settingsOffset) override {
    }

    void onReadFile(ReadFileEvent &event) {
        const std::string &p = event.getpath().getUtf8StdString();
        if (SDK::currentScreen == "modal_progress_screen" || SDK::currentScreen == "world_saving_progress_screen")
        {
            event.returnresult(event.result);
            return;
        }

        // Logger::custom(fg(fmt::color::dark_green), "minecraft.windows.exe", "Loaded path: {}", p); //this is pretty cool
        if (p.find("/data/renderer/materials/") != std::string::npos && strncmp(p.c_str() + p.size() - 13, ".material.bin", 13) == 0) {

            if (!_composeFullStackHook::resourcePackManager) {
                //Logger::custom(fg(fmt::color::yellow), "MaterialBinLoader", "Shaders cant be loaded yet ResourcePackManager is null!");
                event.returnresult(event.result);
                return;
            }

            if (!ispatched) {
                ispatched = true;
                // Bypass VendorID check to support some Intel GPUs
                // bgfx::d3d12::RendererContextD3D12::init
                if (auto ptr = reinterpret_cast<uint8_t*>(Memory::findSig("81 BF ?? ?? 00 00 86 80 00 00", "bgfx::d3d12::RendererContextD3D12::init VendorID check")); ptr) {
                    // 1.19.40
                    ScopedVP(ptr, 10, PAGE_READWRITE);
                    ptr[6] = 0;
                    ptr[7] = 0;
                    Logger::custom(fg(fmt::color::green), "MaterialBinLoader", "Successfully patched bgfx::d3d12::RendererContextD3D12::init");
                } else {
                    Logger::custom(fg(fmt::color::red), "MaterialBinLoader", "Failed to patch bgfx::d3d12::RendererContextD3D12::init");
                }

                // Fix rendering issues on some NVIDIA GPUs
                // dragon::bgfximpl::toSamplerFlags
                if (auto ptr = reinterpret_cast<uint8_t*>(Memory::findSig("FF E1 80 7B ? ? B8 00 00 07 10", "dragon::bgfximpl::toSamplerFlags NVIDIA fix")); ptr) {
                    // 1.21.50
                    ScopedVP(ptr, 10, PAGE_READWRITE);
                    ptr[9] = 0;
                    Logger::custom(fg(fmt::color::green), "MaterialBinLoader", "Successfully patched dragon::bgfximpl::toSamplerFlags");
                } else {
                    Logger::custom(fg(fmt::color::red), "MaterialBinLoader", "Failed to patch dragon::bgfximpl::toSamplerFlags");
                }
            }

            std::string binPath = "renderer/materials/" + p.substr(p.find_last_of('/') + 1);
            if (binPath.find("Weather.material.bin") != std::string::npos) { //avoiding due to it causing crashes with newb shaders seems to also be the case with original BRD
                Logger::custom(fg(fmt::color::gray), "MaterialBinLoader", "Skipping Weather.material.bin");
                event.returnresult(event.result);
                return;
            }

            ResourceLocation location(binPath, false);
            std::string out;

            bool success = _composeFullStackHook::resourcePackManagerLoad(_composeFullStackHook::resourcePackManager, location, out);
            if (success && !out.empty()) {
                if (VersionUtils::checkAboveOrEqual(21, 100)) event.result->assign(out);
                    else event.retstr->assign(out);

                Logger::custom(fg(fmt::color::green), "MaterialBinLoader", "Successfully loaded: {}", binPath);
            }
        }
    }
};

