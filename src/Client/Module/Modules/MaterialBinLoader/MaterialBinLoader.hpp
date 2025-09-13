#pragma once

#include "../Module.hpp"
#include "../../../Hook/Hooks/Game/ReadFileHook.hpp"
#include "src/Client/Hook/Hooks/Game/composeFullStack.hpp"

class MaterialBinLoader : public Module {


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
        // Logger::custom(fg(fmt::color::dark_green), "minecraft.windows.exe", "Loaded path: {}", p); //this is pretty cool
        if (p.find("/data/renderer/materials/") != std::string::npos && strncmp(p.c_str() + p.size() - 13, ".material.bin", 13) == 0) {

            if (!_composeFullStackHook::resourcePackManager) {
                Logger::error("Couldn't load materials ResourcePackManager is null!");
                event.returnresult(event.result);
                return;
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
            } else {
                Logger::custom(fg(fmt::color::red), "MaterialBinLoader", "Failed to load: {}", binPath);
            }
        }
    }
};

