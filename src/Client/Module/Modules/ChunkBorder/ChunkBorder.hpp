#pragma once

#include <Modules/Module.hpp>
#include <Utils/Utils.hpp>

#include <Utils/Render/DrawUtil3D.hpp>
#include <Utils/Render/MaterialUtils.hpp>

#include "Events/Render/Render3DEvent.hpp"
#include "../../../../Assets/Assets.hpp"

class ChunkBorder : public Module {
private:
    float bottom = -64.0f;
    float top = 320.0f;
    int chunkSize = 16.0f;

public:
    ChunkBorder() : Module("Chunk Border", "Chunk Border",
        IDR_CHUNKBORDER_PNG, "", false, {"java", "chunkmap"}) {

    };

    void onEnable() override;

    void onDisable() override;

    void defaultConfig() override;

    void settingsRender(float settingsOffset) override;

    void onRender3D(Render3DEvent& event);

};
