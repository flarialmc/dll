#include "Hitbox.hpp"

void Hitbox::onEnable() {
    Listen(this, KeyEvent, &Hitbox::onKey)
    Listen(this, SetupAndRenderEvent, &Hitbox::onSetupAndRender)
    Listen(this, RenderEvent, &Hitbox::onRender)
    Listen(this, PerspectiveEvent, &Hitbox::onPerspectiveChange)
    Module::onEnable();
}

void Hitbox::onDisable() {
    Deafen(this, KeyEvent, &Hitbox::onKey)
    Deafen(this, SetupAndRenderEvent, &Hitbox::onSetupAndRender)
    Deafen(this, RenderEvent, &Hitbox::onRender)
    Deafen(this, PerspectiveEvent, &Hitbox::onPerspectiveChange)
    Module::onDisable();
}

void Hitbox::defaultConfig() {
    settings.renameSetting("color", "colorOpacity", "color_rgb", "hitbox");

    getKeybind();

    Module::defaultConfig("core");
    // Java Edition default colors
    setDef("hitbox", (std::string)"FFFFFF", 1.0f, false);      // White
    setDef("eyeLineColor", (std::string)"FF0000", 1.0f, false); // Red
    setDef("lookLineColor", (std::string)"0000FF", 1.0f, false); // Blue
    setDef("thickness", 1.1f);
    setDef("staticThickness", false);
    setDef("outline", false);
    setDef("eyeLine", true);
    setDef("lookLine", true);
    setDef("lookLineLength", 2.0f);
    setDef("showSelf", true);
    setDef("hitboxOpacity", 1.0f);

    setDef("toggle", true);
}

void Hitbox::settingsRender(float settingsOffset) {

    initSettingsPage();

    addToggle("Toggle (Java-like behaviour)", "", "toggle");
    addKeybind("Keybind", "", "keybind", true);
    extraPadding();

    addToggle("2D Mode", "You get 2D rectangles instead of cuboids.", "outline");
    addToggle("Static Thickness", "Keeps line thickness constant regardless of distance.", "staticThickness");
    addSlider("Thickness", "Pretty much self-explanatory.", "thickness", 5.0f, 0.1f, false);
    addSlider("Opacity", "Adjust how transparent or opaque the hitbox is.", "hitboxOpacity", 1.0f, 0.0f, false);
    extraPadding();

    addHeader("Java Edition Features");
    addToggle("Eye Line", "Shows a rectangle at eye level", "eyeLine");
    addToggle("Look Direction", "Shows a line indicating where the entity is looking", "lookLine");
    addConditionalSlider(getOps<bool>("lookLine"), "Look Line Length", "How far the line of sight extends", "lookLineLength", 10.0f, 0.5f, false);
    addToggle("Show Self", "Shows your own hitbox when in third person", "showSelf");
    addToggle("Toggle Keybind", "Also self-explanatory. Press the bind to toggle.", "toggle");
    addConditionalKeybind(getOps<bool>("toggle"), "Keybind", "", "keybind", true);
    extraPadding();

    addHeader("Colors");
    addColorPicker("Hitbox", "", "hitbox");
    addColorPicker("Eye Line", "", "eyeLineColor");
    addColorPicker("Look Direction", "", "lookLineColor");

    FlarialGUI::UnsetScrollView();

    resetPadding();

}

void Hitbox::onKey(KeyEvent &event) {
    if (!this->isEnabled()) return;
    if (this->isKeyPartOfKeybind(event.key)) {
        if (this->isKeybind(event.keys)) {
            keybindActions[0]({});
        }
    }
}

void Hitbox::onSetupAndRender(SetupAndRenderEvent &event) {
    if (!this->isEnabled()) return;
    std::lock_guard<std::mutex> guard(renderMtx);
    aabbsToRender.clear();
    if (!SDK::clientInstance || !SDK::clientInstance->getLocalPlayer() || SDK::getCurrentScreen() != "hud_screen" ||
        !SDK::clientInstance->getLocalPlayer()->getLevel())
        return;

    auto player = SDK::clientInstance->getLocalPlayer();
    auto selectedEntity = player->getLevel()->getHitResult().getEntity();
    auto actorList = player->getLevel()->getRuntimeActorList();

    bool showSelf = getOps<bool>("showSelf");
    bool isThirdPerson = currentPerspective != Perspective::FirstPerson;

    auto addEntityInfo = [&](Actor* ent, bool isSelected) {
        auto aabb = ent->getLerpedAABB();
        auto hitbox = ent->getLerpedAABB(true);

        float entityHeight = aabb.upper.y - aabb.lower.y;
        float eyeHeight = aabb.lower.y + entityHeight * 0.85f;
        float centerX = (aabb.lower.x + aabb.upper.x) * 0.5f;
        float centerZ = (aabb.lower.z + aabb.upper.z) * 0.5f;
        Vec3<float> eyePos(centerX, eyeHeight, centerZ);

        Vec2<float> rotation(0.f, 0.f);
        auto rotComp = ent->getActorRotationComponent();
        if (rotComp) {
            rotation = rotComp->rot;
        }

        AABBInfo info = { aabb, hitbox, isSelected, eyePos, rotation };
        aabbsToRender.emplace_back(info);
    };

    // Add local player hitbox when in third person (Java Edition behavior)
    if (showSelf && isThirdPerson) {
        if (player->isValid() && player->isValidAABB()) {
            addEntityInfo(player, false);
        }
    }

    for (const auto& ent : actorList) {
        if (!ent) continue;
        if (ent == player) continue;

        auto pos = ent->getPosition();
        if (!pos) continue;

        if (!ent->hasCategory(ActorCategory::Mob)) continue;
        if (!ent->isValidAABB()) continue;

        float dist = player->getPosition()->dist(*pos);
        if (dist > 30) continue;

        if (ent->getActorFlag(ActorFlags::FLAG_INVISIBLE)) continue;

        if (!player->canSee(*ent)) continue;

        addEntityInfo(ent, selectedEntity == ent);
    }
}

void Hitbox::onPerspectiveChange(PerspectiveEvent &event) {
    currentPerspective = event.getPerspective();
}

void Hitbox::onRender(RenderEvent &event) {
    if (!this->isEnabled()) return;
    if (!SDK::clientInstance || !SDK::clientInstance->getLocalPlayer() || SDK::getCurrentScreen() != "hud_screen" ||
        !SDK::clientInstance->getLocalPlayer()->getLevel() || ModuleManager::getModule("ClickGUI")->active || FlarialGUI::inMenu)
        return;

    auto player = SDK::clientInstance->getLocalPlayer();

    if (player != nullptr) {
        std::lock_guard<std::mutex> guard(renderMtx);

        for (const auto& aabbInfo : aabbsToRender) {
            float thickness = getOps<float>("thickness");
            bool staticThickness = getOps<bool>("staticThickness");
            bool outline = getOps<bool>("outline");
            bool showEyeLine = getOps<bool>("eyeLine");
            bool showLookLine = getOps<bool>("lookLine");

            float lineWidth = thickness;

            if (!staticThickness) {
                float distance = player->getRenderPositionComponent()->renderPos.dist(aabbInfo.aabb.lower);
                float scaleFactor = 1.f - (distance / 30.0f);
                lineWidth = thickness * scaleFactor;
            }

            D2D_COLOR_F hitboxColor = getColor("hitbox");
            hitboxColor.a *= getOps<float>("hitboxOpacity");
            DrawUtils::addBox(aabbInfo.aabb.lower, aabbInfo.aabb.upper, staticThickness ? thickness : lineWidth, outline ? 2 : 1, hitboxColor);

            // Draw eye height indicator (4 lines forming rectangle at eye level) - Java Edition style
            if (showEyeLine) {
                float minX = aabbInfo.aabb.lower.x;
                float maxX = aabbInfo.aabb.upper.x;
                float minZ = aabbInfo.aabb.lower.z;
                float maxZ = aabbInfo.aabb.upper.z;
                float eyeY = aabbInfo.eyePos.y;

                D2D_COLOR_F eyeColor = getColor("eyeLineColor");

                DrawUtils::addLine3D(Vec3<float>(minX, eyeY, minZ), Vec3<float>(maxX, eyeY, minZ), staticThickness ? thickness : lineWidth, eyeColor);
                DrawUtils::addLine3D(Vec3<float>(maxX, eyeY, minZ), Vec3<float>(maxX, eyeY, maxZ), staticThickness ? thickness : lineWidth, eyeColor);
                DrawUtils::addLine3D(Vec3<float>(maxX, eyeY, maxZ), Vec3<float>(minX, eyeY, maxZ), staticThickness ? thickness : lineWidth, eyeColor);
                DrawUtils::addLine3D(Vec3<float>(minX, eyeY, maxZ), Vec3<float>(minX, eyeY, minZ), staticThickness ? thickness : lineWidth, eyeColor);
            }

            // Draw look direction line (blue line from eye position) - Java Edition style
            if (showLookLine) {
                static constexpr float PI = 3.1415927f;
                static constexpr float DEG_TO_RAD = PI / 180.0f;

                float yawRad = aabbInfo.rotation.y * DEG_TO_RAD;
                float pitchRad = aabbInfo.rotation.x * DEG_TO_RAD;

                float dx = -sinf(yawRad) * cosf(pitchRad);
                float dy = -sinf(pitchRad);
                float dz = cosf(yawRad) * cosf(pitchRad);

                float lineLength = getOps<float>("lookLineLength");
                Vec3<float> lookEnd(
                    aabbInfo.eyePos.x + dx * lineLength,
                    aabbInfo.eyePos.y + dy * lineLength,
                    aabbInfo.eyePos.z + dz * lineLength
                );

                D2D_COLOR_F lookColor = getColor("lookLineColor");
                DrawUtils::addLine3D(aabbInfo.eyePos, lookEnd, staticThickness ? thickness : lineWidth, lookColor);
            }
        }
    }
}
