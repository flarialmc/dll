#include "BlockOutline.hpp"

#include "Events/EventManager.hpp"
#include "Events/Render/RenderOutlineSelectionEvent.hpp"

void BlockOutline::onEnable() {
    options.parseOptionsFile();
    Listen(this, RenderOutlineSelectionEvent, &BlockOutline::onOutlineSelection)
    Module::onEnable();
    if (options.options["gfx_classic_box_selection"] != "1") FlarialGUI::Notify(
        "Enable 'Outline Selection' in Minecraft Video Settings for this to work.");
}

void BlockOutline::onDisable() {
    Deafen(this, RenderOutlineSelectionEvent, &BlockOutline::onOutlineSelection)
    Module::onDisable();
}

void BlockOutline::defaultConfig() {
    settings.renameSetting("color", "colorOpacity", "color_rgb", "outline");
    settings.renameSetting("overlayfullblock", "fulloverlay");
    settings.renameSetting("showfulloutline", "fulloutline");
    settings.renameSetting("overlay", "showoverlay");

    Module::defaultConfig("core");
    setDef("outline", "FFFFFF", 1.0f, false);
    setDef("overlay", "FFFFFF", 0.6f, false);

    setDef("showoutline", true);
    setDef("showoverlay", true);

    setDef("fulloutline", true);
    setDef("fulloverlay", false);
    setDef("outlinewidth", 0.01f);
}

void BlockOutline::settingsRender(float settingsOffset) {
    initSettingsPage();

    addHeader("Outline");
    addToggle("Enabled", "", "showoutline");
    addToggle("3D Outline", "Shows the outline through blocks", "fulloutline");
    addColorPicker("Color", "", "outline");
    addSlider("Width", "Thickness of the outline", "outlinewidth", 0.5f);


    addHeader("Overlay");
    addToggle("Enabled", "", "showoverlay");
    addToggle("3D Overlay", "Overlays the full block", "fulloverlay");
    addColorPicker("Color", "", "overlay");

    FlarialGUI::UnsetScrollView();

    resetPadding();
}

void BlockOutline::onOutlineSelection(RenderOutlineSelectionEvent &event) {
    if (!this->isEnabled()) return;
    D2D1_COLOR_F outlineCol = getColor("outline");
    D2D1_COLOR_F overlayCol = getColor("overlay");

    mce::MaterialPtr *material = MaterialUtils::getNametag();

    MCDrawUtil3D dc(SDK::clientInstance->getLevelRender(), event.getScreenContext(), material);

    auto face = (int) SDK::clientInstance->getLocalPlayer()->getLevel()->getHitResult().facing;

    auto bp = event.getPos();

    if (getOps<bool>("showoutline")) drawOutline(dc, bp, outlineCol, getOps<bool>("fulloutline"), getOps<float>("outlinewidth"));

    if (getOps<bool>("showoverlay")) drawOverlay(dc, bp, overlayCol, face, getOps<bool>("fulloverlay"));

    dc.flush();

    event.cancel();
}

void BlockOutline::drawOverlay(MCDrawUtil3D dc, Vec3<float> bp, D2D_COLOR_F col, auto face, bool full) {
    bp.sub(0.0015);

    // The else block remains unchanged as it draws full faces without offsets
    auto drawUp = [&]() {
        dc.fillQuad(
        Vec3<float>(bp.x, bp.y + 1.003f, bp.z),
        Vec3<float>(bp.x + 1.003f, bp.y + 1.003f, bp.z),
        Vec3<float>(bp.x + 1.003f, bp.y + 1.003f, bp.z + 1.003f),
        Vec3<float>(bp.x, bp.y + 1.003f, bp.z + 1.003f),
        col
        );
    };

    auto drawDown = [&]() {
        // y offset (bugged side)
        dc.fillQuad(
            Vec3<float>(bp.x, bp.y - 0.001f, bp.z),
            Vec3<float>(bp.x + 1.003f, bp.y - 0.001f, bp.z),
            Vec3<float>(bp.x + 1.003f, bp.y - 0.001f, bp.z + 1.003f),
            Vec3<float>(bp.x, bp.y - 0.001f, bp.z + 1.003f),
            col
        );
    };

    auto drawEast = [&]() {
        dc.fillQuad(
            Vec3<float>(bp.x + 1.003f, bp.y, bp.z),
            Vec3<float>(bp.x + 1.003f, bp.y + 1.003f, bp.z),
            Vec3<float>(bp.x + 1.003f, bp.y + 1.003f, bp.z + 1.003f),
            Vec3<float>(bp.x + 1.003f, bp.y, bp.z + 1.003f),
            col
        );
    };

    auto drawWest = [&]() {
        // x offset (bugged side)
        dc.fillQuad(
            Vec3<float>(bp.x - 0.001f, bp.y, bp.z),
            Vec3<float>(bp.x - 0.001f, bp.y + 1.003f, bp.z),
            Vec3<float>(bp.x - 0.001f, bp.y + 1.003f, bp.z + 1.003f),
            Vec3<float>(bp.x - 0.001f, bp.y, bp.z + 1.003f),
            col
        );
    };

    auto drawSouth = [&]() {
        dc.fillQuad(
            Vec3<float>(bp.x, bp.y, bp.z + 1.003f),
            Vec3<float>(bp.x, bp.y + 1.003f, bp.z + 1.003f),
            Vec3<float>(bp.x + 1.003f, bp.y + 1.003f, bp.z + 1.003f),
            Vec3<float>(bp.x + 1.003f, bp.y, bp.z + 1.003f),
            col
        );
    };

    auto drawNorth = [&]() {
        // z offset (bugged side)
        dc.fillQuad(
            Vec3<float>(bp.x, bp.y, bp.z - 0.001f),
            Vec3<float>(bp.x, bp.y + 1.003f, bp.z - 0.001f),
            Vec3<float>(bp.x + 1.003f, bp.y + 1.003f, bp.z - 0.001f),
            Vec3<float>(bp.x + 1.003f, bp.y, bp.z - 0.001f),
            col
        );
    };

    if (!full) {
        switch (face) {
            case 0: drawDown(); break;
            case 1: drawUp(); break;
            case 2: drawNorth(); break;
            case 3: drawSouth(); break;
            case 4: drawWest(); break;
            case 5: drawEast(); break;
        }
    } else {
        drawNorth();
        drawEast();
        drawSouth();
        drawWest();
        drawUp();
        drawDown();
    }
}

void BlockOutline::drawOutline(MCDrawUtil3D dc, Vec3<float> bp, D2D_COLOR_F col, bool full, float width) {
    if (full) dc.setMaterial(MaterialUtils::getParticlesAlpha());
    else dc.setMaterial(MaterialUtils::getNametag());

    auto drawUp = [&](float rwidth) {
        float w = rwidth;
        float wi = 1 - w;
        float y_base = bp.y + 1.001f; // Offset y-coordinate outward

        dc.fillQuad(Vec3<float>(bp.x, y_base, bp.z),
                    Vec3<float>(bp.x + 1.002f, y_base, bp.z),
                    Vec3<float>(bp.x + 1.002f, y_base, bp.z + w),
                    Vec3<float>(bp.x, y_base, bp.z + w), col);
        // Top Right (back edge): Extend x to meet south face
        dc.fillQuad(Vec3<float>(bp.x, y_base, bp.z + wi),
                    Vec3<float>(bp.x + 1.002f, y_base, bp.z + wi),
                    Vec3<float>(bp.x + 1.002f, y_base, bp.z + 1),
                    Vec3<float>(bp.x, y_base, bp.z + 1), col);
        // Top (right edge): Extend z to meet east face
        dc.fillQuad(Vec3<float>(bp.x + wi, y_base, bp.z + w),
                    Vec3<float>(bp.x + 1.002f, y_base, bp.z + w),
                    Vec3<float>(bp.x + 1.002f, y_base, bp.z + wi),
                    Vec3<float>(bp.x + wi, y_base, bp.z + wi), col);
        // Top Bottom (left edge): Extend z to meet west face
        dc.fillQuad(Vec3<float>(bp.x, y_base, bp.z + w),
                    Vec3<float>(bp.x + w, y_base, bp.z + w),
                    Vec3<float>(bp.x + w, y_base, bp.z + wi),
                    Vec3<float>(bp.x, y_base, bp.z + wi), col);
    };

    auto drawDown = [&](float rwidth) {
        float w = rwidth;
        float wi = 1 - w;
        float y_base = bp.y; // Offset y-coordinate outward
        dc.fillQuad(Vec3<float>(bp.x, y_base, bp.z),
                    Vec3<float>(bp.x + 1.002f, y_base, bp.z),
                    Vec3<float>(bp.x + 1.002f, y_base, bp.z + w),
                    Vec3<float>(bp.x, y_base, bp.z + w), col);
        // Top Right (back edge): Extend x to meet south face
        dc.fillQuad(Vec3<float>(bp.x, y_base, bp.z + wi),
                    Vec3<float>(bp.x + 1.002f, y_base, bp.z + wi),
                    Vec3<float>(bp.x + 1.002f, y_base, bp.z + 1),
                    Vec3<float>(bp.x, y_base, bp.z + 1), col);
        // Top Top (right edge): Extend z to meet east face
        dc.fillQuad(Vec3<float>(bp.x + wi, y_base, bp.z + w),
                    Vec3<float>(bp.x + 1.002f, y_base, bp.z + w),
                    Vec3<float>(bp.x + 1.002f, y_base, bp.z + wi),
                    Vec3<float>(bp.x + wi, y_base, bp.z + wi), col);
        // Top Bottom (left edge): Extend z to meet west face
        dc.fillQuad(Vec3<float>(bp.x, y_base, bp.z + w),
                    Vec3<float>(bp.x + w, y_base, bp.z + w),
                    Vec3<float>(bp.x + w, y_base, bp.z + wi),
                    Vec3<float>(bp.x, y_base, bp.z + wi), col);
    };

    auto drawEast = [&](float rwidth) {
        float w = rwidth;
        float wi = 1 - w;
        float x_base = bp.x + 1.001f;
        // Bottom strip (along y = bp.y)
        dc.fillQuad(Vec3<float>(x_base, bp.y, bp.z), Vec3<float>(x_base, bp.y + w, bp.z),
                    Vec3<float>(x_base, bp.y + w, bp.z + 1.002f), Vec3<float>(x_base, bp.y, bp.z + 1.002f), col);
        // Top strip (along y = bp.y + 1.002f)
        dc.fillQuad(Vec3<float>(x_base, bp.y + 1.002f - w, bp.z), Vec3<float>(x_base, bp.y + 1.002f, bp.z),
                    Vec3<float>(x_base, bp.y + 1.002f, bp.z + 1.002f),
                    Vec3<float>(x_base, bp.y + 1.002f - w, bp.z + 1.002f), col);
        // Front strip (along z = bp.z)
        dc.fillQuad(Vec3<float>(x_base, bp.y + w, bp.z), Vec3<float>(x_base, bp.y + 1.002f - w, bp.z),
                    Vec3<float>(x_base, bp.y + 1.002f - w, bp.z + w), Vec3<float>(x_base, bp.y + w, bp.z + w),
                    col);
        // Back strip (along z = bp.z + 1.002f)
        dc.fillQuad(Vec3<float>(x_base, bp.y + w, bp.z + 1.002f - w),
                    Vec3<float>(x_base, bp.y + 1.002f - w, bp.z + 1.002f - w),
                    Vec3<float>(x_base, bp.y + 1.002f - w, bp.z + 1.002f),
                    Vec3<float>(x_base, bp.y + w, bp.z + 1.002f), col);
    };

    auto drawWest = [&](float rwidth) {
        float w = rwidth;
        float wi = 1 - w;
        float x_base = bp.x - 0.001f; // Offset x-coordinate outward
        // Bottom strip (along y = bp.y)
        dc.fillQuad(Vec3<float>(x_base, bp.y, bp.z), Vec3<float>(x_base, bp.y + w, bp.z),
                    Vec3<float>(x_base, bp.y + w, bp.z + 1.002f), Vec3<float>(x_base, bp.y, bp.z + 1.002f), col);
        // Top strip (along y = bp.y + 1.002f)
        dc.fillQuad(Vec3<float>(x_base, bp.y + 1.002f - w, bp.z), Vec3<float>(x_base, bp.y + 1.002f, bp.z),
                    Vec3<float>(x_base, bp.y + 1.002f, bp.z + 1.002f),
                    Vec3<float>(x_base, bp.y + 1.002f - w, bp.z + 1.002f), col);
        // Front strip (along z = bp.z)
        dc.fillQuad(Vec3<float>(x_base, bp.y + w, bp.z), Vec3<float>(x_base, bp.y + 1.002f - w, bp.z),
                    Vec3<float>(x_base, bp.y + 1.002f - w, bp.z + w), Vec3<float>(x_base, bp.y + w, bp.z + w),
                    col);
        // Back strip (along z = bp.z + 1.002f)
        dc.fillQuad(Vec3<float>(x_base, bp.y + w, bp.z + 1.002f - w),
                    Vec3<float>(x_base, bp.y + 1.002f - w, bp.z + 1.002f - w),
                    Vec3<float>(x_base, bp.y + 1.002f - w, bp.z + 1.002f),
                    Vec3<float>(x_base, bp.y + w, bp.z + 1.002f), col);
    };

    auto drawSouth = [&](float rwidth) {
        float w = rwidth;
        float z_base = bp.z + 1.001f; // Offset z-coordinate outward
        // Bottom strip (along y = bp.y, z = bp.z + 1.f)
        dc.fillQuad(Vec3<float>(bp.x, bp.y, z_base), Vec3<float>(bp.x + 1.002f, bp.y, z_base),
                    Vec3<float>(bp.x + 1.002f, bp.y + w, z_base), Vec3<float>(bp.x, bp.y + w, z_base), col);
        // Top strip (along y = bp.y + 1.002f, z = bp.z + 1.002f)
        dc.fillQuad(Vec3<float>(bp.x, bp.y + 1.002f - w, z_base),
                    Vec3<float>(bp.x + 1.002f, bp.y + 1.002f - w, z_base),
                    Vec3<float>(bp.x + 1.002f, bp.y + 1.002f, z_base), Vec3<float>(bp.x, bp.y + 1.002f, z_base),
                    col);
        // Left strip (along x = bp.x, z = bp.z + 1.002f)
        dc.fillQuad(Vec3<float>(bp.x, bp.y + w, z_base), Vec3<float>(bp.x, bp.y + 1.002f - w, z_base),
                    Vec3<float>(bp.x + w, bp.y + 1.002f - w, z_base), Vec3<float>(bp.x + w, bp.y + w, z_base),
                    col);
        // Right strip (along x = bp.x + 1.002f, z = bp.z + 1.002f)
        dc.fillQuad(Vec3<float>(bp.x + 1.002f - w, bp.y + w, z_base),
                    Vec3<float>(bp.x + 1.002f - w, bp.y + 1.002f - w, z_base),
                    Vec3<float>(bp.x + 1.002f, bp.y + 1.002f - w, z_base),
                    Vec3<float>(bp.x + 1.002f, bp.y + w, z_base), col);
    };

    auto drawNorth = [&](float rwidth) {
        float w = rwidth;
        float z_base = bp.z - 0.001f; // Offset z-coordinate outward
        // Bottom strip (along y = bp.y, z = bp.z)
        dc.fillQuad(Vec3<float>(bp.x, bp.y, z_base), Vec3<float>(bp.x + 1.002f, bp.y, z_base),
                    Vec3<float>(bp.x + 1.002f, bp.y + w, z_base), Vec3<float>(bp.x, bp.y + w, z_base), col);
        // Top strip (along y = bp.y + 1.002f, z = bp.z)
        dc.fillQuad(Vec3<float>(bp.x, bp.y + 1.002f - w, z_base),
                    Vec3<float>(bp.x + 1.002f, bp.y + 1.002f - w, z_base),
                    Vec3<float>(bp.x + 1.002f, bp.y + 1.002f, z_base), Vec3<float>(bp.x, bp.y + 1.002f, z_base),
                    col);
        // Left strip (along x = bp.x, z = bp.z)
        dc.fillQuad(Vec3<float>(bp.x, bp.y + w, z_base), Vec3<float>(bp.x, bp.y + 1.002f - w, z_base),
                    Vec3<float>(bp.x + w, bp.y + 1.002f - w, z_base), Vec3<float>(bp.x + w, bp.y + w, z_base),
                    col);
        // Right strip (along x = bp.x + 1.002f, z = bp.z)
        dc.fillQuad(Vec3<float>(bp.x + 1.002f - w, bp.y + w, z_base),
                    Vec3<float>(bp.x + 1.002f - w, bp.y + 1.002f - w, z_base),
                    Vec3<float>(bp.x + 1.002f, bp.y + 1.002f - w, z_base),
                    Vec3<float>(bp.x + 1.002f, bp.y + w, z_base), col);
    };

    drawNorth(width);
    drawEast(width);
    drawSouth(width);
    drawWest(width);
    drawUp(width);
    drawDown(width);
}