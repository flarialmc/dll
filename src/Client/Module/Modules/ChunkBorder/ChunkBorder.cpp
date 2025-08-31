#include "ChunkBorder.hpp"

#include "Events/EventManager.hpp"
#include "Events/Render/Render3DEvent.hpp"
#include <Utils/Render/DrawUtil3D.hpp>
#include "Utils/Render/MaterialUtils.hpp"
#include <cmath>

void ChunkBorder::onEnable() {
    Listen(this, Render3DEvent, &ChunkBorder::onRender3D)
    Module::onEnable();
}

void ChunkBorder::onDisable() {
    Deafen(this, Render3DEvent, &ChunkBorder::onRender3D)
    Module::onDisable();
}

void ChunkBorder::defaultConfig() {
    Module::defaultConfig("core");
    setDef("corner", (std::string) "0000ff", 1.f, false);
    setDef("mid", (std::string) "ffff00", 1.f, false);
    setDef("adj", (std::string) "ff0000", 1.f, false);
    setDef("horizLineSpacing", 2);
    setDef("vertLineSpacing", 2.f);
}

void ChunkBorder::settingsRender(float settingsOffset) {
    initSettingsPage();

    addHeader("Chunk Border");
    addSliderInt("Horizontal Line Spacing", "", "horizLineSpacing", chunkSize, 1);
    addSlider("Vertical Line Spacing", "", "vertLineSpacing", 20.f, 1.f);

    addHeader("Line Colors");
    addColorPicker("Main Line Color", "", "mid");
    addColorPicker("Adjacent Chunks Line Color", "", "adj");
    addColorPicker("Corner Line Color", "", "corner");

    FlarialGUI::UnsetScrollView();

    resetPadding();
}

void ChunkBorder::onRender3D(Render3DEvent &event) {
    if (!this->isEnabled()) return;

    const auto player = SDK::clientInstance->getLocalPlayer();
    if (!player) return;

    const auto pos = player->getLerpedPosition();
    mce::MaterialPtr *material = MaterialUtils::getSelectionOverlay();
    MCDrawUtil3D dc(event.LevelRenderer, event.screenContext, material);

    const Vec3<float> anchor = Vec3{
        std::floor(pos.x / 16.0f) * 16.0f,
        0.0f,
        std::floor(pos.z / 16.0f) * 16.0f
    };

    const D2D1_COLOR_F cornerCol = getColor("corner");
    const D2D1_COLOR_F midCol = getColor("mid");
    const D2D1_COLOR_F adjCol = getColor("adj");

    const float vls = getOps<float>("vertLineSpacing");
    const int hls = getOps<int>("horizLineSpacing");

    // CURRENT CHUNK START

    // vertical lines
    for (int i = 0; i <= chunkSize / hls; ++i) {
        const float offset = i * hls;
        D2D1_COLOR_F col = (i % (chunkSize / hls) == 0) ? cornerCol : midCol;

        // z axis lines
        dc.drawLineList(Vec3{anchor.x + offset, bottom, anchor.z}, Vec3{anchor.x + offset, top, anchor.z}, col);
        dc.drawLineList(Vec3{anchor.x + offset, bottom, anchor.z + chunkSize}, Vec3{anchor.x + offset, top, anchor.z + chunkSize}, col);

        // x axis lines
        dc.drawLineList(Vec3{anchor.x, bottom, anchor.z + offset}, Vec3{anchor.x, top, anchor.z + offset}, col);
        dc.drawLineList(Vec3{anchor.x + chunkSize, bottom, anchor.z + offset}, Vec3{anchor.x + chunkSize, top, anchor.z + offset}, col);
    }

    // horizontal lines
    for (float y = bottom; y <= top; y += vls) {
        dc.drawLineList(Vec3{anchor.x, y, anchor.z}, Vec3{anchor.x + chunkSize, y, anchor.z}, midCol);
        dc.drawLineList(Vec3{anchor.x, y, anchor.z}, Vec3{anchor.x, y, anchor.z + chunkSize}, midCol);
        dc.drawLineList(Vec3{anchor.x + chunkSize, y, anchor.z}, Vec3{anchor.x + chunkSize, y, anchor.z + chunkSize}, midCol);
        dc.drawLineList(Vec3{anchor.x, y, anchor.z + chunkSize}, Vec3{anchor.x + chunkSize, y, anchor.z + chunkSize}, midCol);
    }

    // CURRENT CHUNK END

    // adjacent chunks
    for (int x = -1; x <= 2; ++x) {
        for (int z = -1; z <= 2; ++z) {
            if (x >= 0 && x <= 1 && z >= 0 && z <= 1) continue;

            dc.drawLineList(
                Vec3{anchor.x + x * chunkSize, bottom, anchor.z + z * chunkSize},
                Vec3{anchor.x + x * chunkSize, top, anchor.z + z * chunkSize},
                adjCol
            );
        }
    }

    dc.flush();
}
