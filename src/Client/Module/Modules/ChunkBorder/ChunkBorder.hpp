#pragma once

#include <Modules/Module.hpp>
#include <Utils/Utils.hpp>

#include <Utils/Render/DrawUtil3D.hpp>
#include <Utils/Render/MaterialUtils.hpp>

#include "Events/Render/Render3DEvent.hpp"

class ChunkBorder : public Module {

public:
    ChunkBorder() : Module("Chunk Border", "Chunk Border", IDR_SKULL_PNG, "") {

    };

    void onEnable() override;

    void onDisable() override;

    void defaultConfig() override;

    void settingsRender(float settingsOffset) override;

    void onRender3D(Render3DEvent& event);

};
