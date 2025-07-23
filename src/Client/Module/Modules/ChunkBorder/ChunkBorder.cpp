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

}

void ChunkBorder::settingsRender(float settingsOffset) {
    float x = Constraints::PercentageConstraint(0.019, "left");
    float y = Constraints::PercentageConstraint(0.10, "top");

    const float scrollviewWidth = Constraints::RelativeConstraint(0.12, "height", true);


    FlarialGUI::ScrollBar(x, y, 140, Constraints::SpacingConstraint(5.5, scrollviewWidth), 2);
    FlarialGUI::SetScrollView(x - settingsOffset, Constraints::PercentageConstraint(0.00, "top"),
                              Constraints::RelativeConstraint(1.0, "width"),
                              Constraints::RelativeConstraint(0.88f, "height"));

    FlarialGUI::UnsetScrollView();

    resetPadding();
}

void ChunkBorder::onRender3D(Render3DEvent &event) {
    if (!this->isEnabled()) return;

    auto player = SDK::clientInstance->getLocalPlayer();

    if (!player) return;

    auto pos = player->getLerpedPosition();

    mce::MaterialPtr *material = MaterialUtils::getSelectionOverlay();

    MCDrawUtil3D dc(event.LevelRenderer, event.screenContext, material);

    Vec3<float> anchor = Vec3<float>{std::floor(pos.x / 16.f) * 16.f, 0.f, std::floor(pos.z / 16.f) * 16.f};
    float bottom = -64.f;
    float top = 320.f;

    D2D1_COLOR_F cornerCol = D2D1_COLOR_F(0.f, 0.f, 1.f, 1.f);
    D2D1_COLOR_F midCol = D2D1_COLOR_F(1.f, 1.f, 0.f, 1.f);
    D2D1_COLOR_F outCol = D2D1_COLOR_F(1.f, 0.f, 0.f, 1.f);

    auto drawHorizontalLines = [&]() {
        for (int i = 0; i <= 8; i++) {

            D2D1_COLOR_F col;
            if (i % 8 == 0) col = cornerCol;
            else col = midCol;

            float offset = i * 2.f;

            dc.drawLineList(
                Vec3<float>{anchor.x + offset, bottom, anchor.z},
                Vec3<float>{anchor.x + offset, top, anchor.z},
                col
            );

            dc.drawLineList(
                Vec3<float>{anchor.x, bottom, anchor.z + offset},
                Vec3<float>{anchor.x, top, anchor.z + offset},
                col
            );
            dc.drawLineList(
                Vec3<float>{anchor.x + 16.f, bottom, anchor.z + offset},
                Vec3<float>{anchor.x + 16.f, top, anchor.z + offset},
                col
            );
            dc.drawLineList(
                Vec3<float>{anchor.x + offset, bottom, anchor.z  + 16.f},
                Vec3<float>{anchor.x + offset, top, anchor.z  + 16.f},
                col
            );
        }
    };

    auto drawVerticalLines = [&]() {
        for (int i = 0; i <= std::floor((top - bottom) /  2); i++) {

            D2D1_COLOR_F col;
            if (i % 8 == 0) col = cornerCol;
            else col = midCol;

            float offset = i * 2.f;

            dc.drawLineList(
                Vec3<float>{anchor.x, bottom + offset, anchor.z},
                Vec3<float>{anchor.x + 16.f, bottom + offset, anchor.z},
                col
            );

            dc.drawLineList(
                Vec3<float>{anchor.x, bottom + offset, anchor.z},
                Vec3<float>{anchor.x, bottom + offset, anchor.z + 16.f},
                col
            );

            dc.drawLineList(
                Vec3<float>{anchor.x + 16.f, bottom + offset, anchor.z},
                Vec3<float>{anchor.x + 16.f, bottom + offset, anchor.z + 16.f},
                col
            );

            dc.drawLineList(
                Vec3<float>{anchor.x, bottom + offset, anchor.z + 16.f},
                Vec3<float>{anchor.x + 16.f, bottom + offset, anchor.z + 16.f},
                col
            );
        }
    };

    auto drawOutChunks = [&]() {
        std::vector<std::pair<float, float>> outChunks = {
            {anchor.x - 16.f, anchor.z + 32.f},
            {anchor.x, anchor.z + 32.f},
            {anchor.x + 16.f, anchor.z + 32.f},
            {anchor.x + 32.f, anchor.z + 32.f},
            {anchor.x - 16.f, anchor.z + 16.f},
            {anchor.x + 32.f, anchor.z + 16.f},
            {anchor.x - 16.f, anchor.z},
            {anchor.x + 32.f, anchor.z},
            {anchor.x - 16.f, anchor.z - 16.f},
            {anchor.x, anchor.z - 16.f},
            {anchor.x + 16.f, anchor.z - 16.f},
            {anchor.x + 32.f, anchor.z - 16.f}
        };

        for (std::pair<float, float>& coords: outChunks) {
            dc.drawLineList(
                Vec3<float>(coords.first, bottom, coords.second),
                Vec3<float>(coords.first, top, coords.second),
                outCol
            );
        }
    };

    drawVerticalLines();
    drawHorizontalLines();
    drawOutChunks();

    dc.flush();

}
