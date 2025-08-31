#include "DirectionHUD.hpp"

#include "Modules/ClickGUI/ClickGUI.hpp"
#include "Modules/Waypoints/Waypoints.hpp"

void DirectionHUD::onEnable() {
    Listen(this, RenderEvent, &DirectionHUD::onRender);
    Module::onEnable();
}

void DirectionHUD::onDisable() {
    Deafen(this, RenderEvent, &DirectionHUD::onRender);
    Module::onDisable();
}

void DirectionHUD::defaultConfig() {
    settings.changeType<float, int>("degreesDecimalCount");
    getKeybind();
    Module::defaultConfig("core");
    Module::defaultConfig("pos");
    Module::defaultConfig("main");
    setDef("moreScales", true);
    setDef("hideWhenTabList", true);
    setDef("pixelsPerDegree", 1.5f);
    setDef("wrapFade", true);
    setDef("fadeDistancePerc", 75.f);
    setDef("showScales", true);
    setDef("scaleShadow", true);
    setDef("scaleShadowOffset", 0.003f);
    setDef("showCardinalScale", true);
    setDef("cardinalScaleWidth", 5.f);
    setDef("showOrdinalScale", true);
    setDef("ordinalScaleWidth", 2.f);
    setDef("showText", true);
    setDef("textShadow", true);
    setDef("textShadowOffset", 0.003f);
    setDef("showCardinalText", true);
    setDef("cardinalTextSize", 1.f);
    setDef("cardinalTextOffset", 1.f);
    setDef("showOrdinalText", true);
    setDef("ordinalTextSize", 0.75f);
    setDef("ordinalTextOffset", 1.f);
    setDef("cardinalScale", (std::string)"fafafa", 1.f, false);
    setDef("ordinalScale", (std::string)"b8b8b8", 1.f, false);
    setDef("cardinalText", (std::string)"fafafa", 1.f, false);
    setDef("ordinalText", (std::string)"b8b8b8", 1.f, false);
    setDef("cardinalTextShadow", (std::string)"000000", 1.f, false);
    setDef("ordinalTextShadow", (std::string)"000000", 1.f, false);
    setDef("cardinalScaleShadow", (std::string)"000000", 1.f, false);
    setDef("ordinalScaleShadow", (std::string)"000000", 1.f, false);
    setDef("showWaypoints", true);
    setDef("showDegrees", true);
    setDef("degreesDecimalCount", 2);
    setDef("degreesTextSize", 1.2f);
    setDef("degreesTextOffset", 6.1f);
    setDef("degreesText", (std::string)"ffffff", 1.f, false);
    setDef("degreesTextShadow", true);
    setDef("degreesTextShadow", (std::string)"000000", 1.f, false);
    setDef("degreesTextShadowOffset", 0.003f);
}

void DirectionHUD::settingsRender(float settingsOffset) {
    initSettingsPage();

    addHeader("Direction HUD");
    defaultAddSettings("main");
    addToggle("Hide on TabList enable", "", "hideWhenTabList");
    addSlider("Pixels Per Degree", "", "pixelsPerDegree", 20.f, 0.001f);
    addToggle("Wrap Around Fade", "", "wrapFade");
    addConditionalSlider(getOps<bool>("wrapFade"), "Fade Distance", "", "fadeDistancePerc", 99, 1);

    extraPadding();

    addHeader("Scales");
    addToggle("Show Scales", "Lines indicating N, E, S, W", "showScales");
    addConditionalToggle(getOps<bool>("showScales"), "MORE SCALES!!", "", "moreScales");

    addConditionalToggle(getOps<bool>("showScales"), "Scale Shadow", "", "scaleShadow");
    addConditionalSlider(getOps<bool>("showScales") && getOps<bool>("scaleShadow"), "Scale Shadow Offset", "How far the shadow will be.", "scaleShadowOffset", 0.02f, 0.001f);

    addConditionalToggle(getOps<bool>("showScales"), "Show Cardinal Scales", "", "showCardinalScale");
    addConditionalSlider(getOps<bool>("showScales") && getOps<bool>("showCardinalScale"), "Cardinal Scale Width", "", "cardinalScaleWidth", 10.0f);

    addConditionalToggle(getOps<bool>("showScales"), "Show Ordinal Scales", "", "showOrdinalScale");
    addConditionalSlider(getOps<bool>("showScales") && getOps<bool>("showOrdinalScale"), "Ordinal Scale Width", "", "ordinalScaleWidth", 10.0f);
    extraPadding();

    addHeader("Text");
    addToggle("Show Text", "Text indicating N, E, S, W", "showText");
    addConditionalToggle(getOps<bool>("showText"), "Text Shadow", "", "textShadow");
    addConditionalSlider(getOps<bool>("showText") && getOps<bool>("textShadow"), "Text Shadow Offset", "How far the shadow will be.", "textShadowOffset", 0.02f, 0.001f);

    addConditionalToggle(getOps<bool>("showText"), "Show Cardinal Text", "", "showCardinalText");
    addConditionalSlider(getOps<bool>("showText") && getOps<bool>("showCardinalText"), "Cardinal Text Size", "", "cardinalTextSize", 5.0f);
    addConditionalSlider(getOps<bool>("showText") && getOps<bool>("showCardinalText"), "Cardinal Text Offset", "", "cardinalTextOffset", 10.0f, 0.0f, false);

    addConditionalToggle(getOps<bool>("showText"), "Show Ordinal Text", "", "showOrdinalText");
    addConditionalSlider(getOps<bool>("showText") && getOps<bool>("showOrdinalText"), "Ordinal Text Size", "", "ordinalTextSize", 5.0f);
    addConditionalSlider(getOps<bool>("showText") && getOps<bool>("showOrdinalText"), "Ordinal Text Offset", "", "ordinalTextOffset", 10.0f, 0.0f, false);

    extraPadding();

    addHeader("Waypoints");
    addToggle("Show Waypoints", "", "showWaypoints");

    extraPadding();

    addHeader("Degrees");
    addToggle("Show Degrees", "Display the exact angle (0-360)", "showDegrees");
    addConditionalSliderInt(getOps<bool>("showDegrees"), "Degrees Decimal Count", "", "degreesDecimalCount", 5, 0);
    addConditionalSlider(getOps<bool>("showDegrees"), "Degrees Text Size", "", "degreesTextSize", 3.0f, 0.5f);
    addConditionalSlider(getOps<bool>("showDegrees"), "Degrees Text Offset", "", "degreesTextOffset", 10.0f, 0.0f, false);
    addConditionalToggle(getOps<bool>("showDegrees"), "Degrees Text Shadow", "", "degreesTextShadow");
    addConditionalSlider(getOps<bool>("showDegrees") && getOps<bool>("degreesTextShadow"), "Degrees Text Shadow Offset", "", "degreesTextShadowOffset", 0.02f, 0.001f);

    extraPadding();

    addHeader("Colors");
    addConditionalColorPicker(getOps<bool>("showText"), "Cardinal Scale Color", "", "cardinalScale");
    addConditionalColorPicker(getOps<bool>("showText"), "Ordinal Scale Color", "", "ordinalScale");
    addConditionalColorPicker(getOps<bool>("showText"), "Cardinal Text Color", "", "cardinalText");
    addConditionalColorPicker(getOps<bool>("showText"), "Ordinal Text Color", "", "ordinalText");
    addConditionalColorPicker(getOps<bool>("showText") && getOps<bool>("textShadow"), "Cardinal Text Shadow Color", "", "cardinalTextShadow");
    addConditionalColorPicker(getOps<bool>("showText") && getOps<bool>("textShadow"), "Ordinal Text Shadow Color", "", "ordinalTextShadow");
    addConditionalColorPicker(getOps<bool>("showDegrees"), "Degrees Text Color", "", "degreesText");
    addConditionalColorPicker(getOps<bool>("showDegrees") && getOps<bool>("degreesTextShadow"), "Degrees Text Shadow Color", "", "degreesTextShadow");
    addConditionalColorPicker(getOps<bool>("showScales") && getOps<bool>("scaleShadow"), "Cardinal Scale Shadow Color", "", "cardinalScaleShadow");
    addConditionalColorPicker(getOps<bool>("showScales") && getOps<bool>("scaleShadow"), "Ordinal Scale Shadow Color", "", "ordinalScaleShadow");

    FlarialGUI::UnsetScrollView();
    resetPadding();
}

float DirectionHUD::calculateDeltaYaw(float currentYaw, float targetYaw) {
    float diff = fmod(targetYaw - currentYaw, 360.0f);
    if (diff < -180.0f) diff += 360.0f;
    else if (diff > 180.0f) diff -= 360.0f;
    return diff;
}

float DirectionHUD::normalizeYaw(float yaw) {
    while (yaw <= -180.0f) yaw += 360.0f;
    while (yaw > 180.0f) yaw -= 360.0f;
    return yaw;
}

float DirectionHUD::getRelativeYaw(float playerX, float playerZ, float pointX, float pointZ, float playerYaw) {
    float pointYaw = std::atan2(playerX - pointX, pointZ - playerZ) * (180.0f / 3.14f);
    return normalizeYaw(pointYaw - playerYaw);
}

void DirectionHUD::onRender(RenderEvent &event) {
    if (!this->isEnabled()) return;
    if (!SDK::clientInstance || !SDK::clientInstance->getLocalPlayer()) return;
    if (SDK::getCurrentScreen() != "hud_screen") return;
    if (getOps<bool>("hideWhenTabList") && ModuleManager::getModule("Tab List") != nullptr && ModuleManager::getModule("Tab List")->active) return;

    LocalPlayer* player = SDK::clientInstance->getLocalPlayer();
    ActorRotationComponent* rotationComponent = player->getActorRotationComponent();

    if (!rotationComponent) return;

    if (rotationComponent->rot.y != 0) lerpYaw = rotationComponent->rot.y;

    // make sure in order as targetYaw
    std::vector<std::string> directions = {
        "N", "E", "S", "W",
        "NW", "NE", "SE", "SW"
    };

    int yawSize = getOps<bool>("moreScales") ? 8 + 16 : 8;

    std::vector<float> deltaYaws(yawSize);
    for (int i = 0; i < yawSize; i++) deltaYaws[i] = calculateDeltaYaw(lerpYaw, targetYaws[i]);

    float uiscale = getOps<float>("uiscale");

    float pixelsPerDegree = Constraints::RelativeConstraint(getOps<float>("pixelsPerDegree") * 0.001f) * getOps<float>("uiscale");
    float fullCirclePixelWidth = 360.0f * pixelsPerDegree;

    float cardinalBarWidth = Constraints::RelativeConstraint(getOps<float>("cardinalScaleWidth") * 0.001f) * uiscale;
    float ordinalBarWidth = Constraints::RelativeConstraint(getOps<float>("ordinalScaleWidth") * 0.001f) * uiscale;

    Vec2<float> settingperc = Vec2<float>(getOps<float>("percentageX"), getOps<float>("percentageY"));
    Vec2<float> realcenter;

    float barHeight = Constraints::RelativeConstraint(0.05f * uiscale);
    float CtextSize = Constraints::SpacingConstraint(5.f, barHeight) * getOps<float>("cardinalTextSize");
    float OtextSize = Constraints::SpacingConstraint(5.f, barHeight) * getOps<float>("ordinalTextSize");

    float cardinalTextOffset = Constraints::RelativeConstraint(getOps<float>("cardinalTextOffset")) * uiscale * 0.01f;
    float ordinalTextOffset = Constraints::RelativeConstraint(getOps<float>("ordinalTextOffset")) * uiscale * 0.01f;

    if (settingperc.x != 0) realcenter = Vec2<float>(settingperc.x * (MC::windowSize.x), settingperc.y * (MC::windowSize.y));
    else realcenter = Constraints::CenterConstraint(fullCirclePixelWidth, barHeight);

    realcenter.x -= fullCirclePixelWidth / 2.f;

    // handle editing in edit menu
    if (ClickGUI::editmenu) {
        FlarialGUI::SetWindowRect(realcenter.x, realcenter.y, fullCirclePixelWidth, barHeight, 34, this->name);

        Vec2<float> vec2 = FlarialGUI::CalculateMovedXY(realcenter.x, realcenter.y, 34, fullCirclePixelWidth, barHeight);

        checkForRightClickAndOpenSettings(realcenter.x, realcenter.y, fullCirclePixelWidth, barHeight);

        vec2.x += fullCirclePixelWidth / 2.f;

        realcenter.x = vec2.x;
        realcenter.y = vec2.y;

        realcenter = realcenter;

        Vec2<float> percentages = Constraints::CalculatePercentage(realcenter.x, realcenter.y, 0, 0);
        this->settings.setValue("percentageX", percentages.x);
        this->settings.setValue("percentageY", percentages.y);

        realcenter.x -= fullCirclePixelWidth / 2.f;
    }

    float hudCenterX = realcenter.x + (fullCirclePixelWidth / 2.f);

    // calc xpos
    std::vector<float> xPos(yawSize);
    for (int i = 0; i < yawSize; i++) xPos[i] = hudCenterX + (deltaYaws[i] * pixelsPerDegree);
    // handle wrapping
    for (int i = 0; i < yawSize; i++) {
        if (xPos[i] > hudCenterX + fullCirclePixelWidth / 2.0f) xPos[i] -= fullCirclePixelWidth;
        if (xPos[i] < hudCenterX - fullCirclePixelWidth / 2.0f) xPos[i] += fullCirclePixelWidth;
    };

    D2D_COLOR_F cardinalScaleCol = getColor("cardinalScale");
    D2D_COLOR_F ordinalScaleCol = getColor("ordinalScale");
    D2D_COLOR_F cardinalTextCol = getColor("cardinalText");
    D2D_COLOR_F ordinalTextCol = getColor("ordinalText");

    D2D_COLOR_F cardinalTextShadowCol = getColor("cardinalTextShadow");
    D2D_COLOR_F ordinalTextShadowCol = getColor("ordinalTextShadow");
    D2D_COLOR_F cardinalScaleShadowCol = getColor("cardinalScaleShadow");
    D2D_COLOR_F ordinalScaleShadowCol = getColor("ordinalScaleShadow");

    // make sure they are in order
    D2D_COLOR_F* cols[] = { &cardinalScaleCol, &ordinalScaleCol, &cardinalTextCol, &ordinalTextCol, &cardinalTextShadowCol, &ordinalTextShadowCol, &cardinalScaleShadowCol, &ordinalScaleShadowCol };
    std::vector<std::string> colStrings = { "cardinalScale", "ordinalScale", "cardinalText", "ordinalText", "cardinalTextShadow", "ordinalTextShadow", "cardinalScaleShadow", "ordinalScaleShadow" };


    // degrees text
    if (getOps<bool>("showDegrees")) {
        // convert to 0-360
        float compassDegrees = fmod((-lerpYaw + 180.0f), 360.0f);
        if (compassDegrees < 0) compassDegrees += 360.0f;

        std::stringstream ss;
        ss << std::fixed << std::setprecision(getOps<int>("degreesDecimalCount")) << compassDegrees;
        std::string degreesText = ss.str();

        // get text size for positioning
        float degreesTextSize = Constraints::SpacingConstraint(5.f, barHeight) * getOps<float>("degreesTextSize");
        float degreesTextOffset = Constraints::RelativeConstraint(getOps<float>("degreesTextOffset") - 5.f) * uiscale * 0.1f;

        // colors
        D2D_COLOR_F degreesTextCol = getColor("degreesText");
        D2D_COLOR_F degreesTextShadowCol = getColor("degreesTextShadow");

        // shadow
        if (getOps<bool>("degreesTextShadow")) {
            FlarialGUI::FlarialTextWithFont(
                hudCenterX + Constraints::RelativeConstraint(getOps<float>("degreesTextShadowOffset")) * uiscale,
                realcenter.y + degreesTextOffset + Constraints::RelativeConstraint(getOps<float>("degreesTextShadowOffset")) * uiscale,
                FlarialGUI::to_wide(degreesText).c_str(),
                0, 0,
                DWRITE_TEXT_ALIGNMENT_CENTER, degreesTextSize,
                DWRITE_FONT_WEIGHT_NORMAL,
                degreesTextShadowCol, true);
        }

        // draw the text itself
        FlarialGUI::FlarialTextWithFont(
            hudCenterX,
            realcenter.y + degreesTextOffset,
            FlarialGUI::to_wide(degreesText).c_str(),
            0, 0,
            DWRITE_TEXT_ALIGNMENT_CENTER, degreesTextSize,
            DWRITE_FONT_WEIGHT_NORMAL,
            degreesTextCol, true);
    }

    for (int i = 0; i < yawSize; i++) {
        int dirType;
        if (i < 4) dirType = 0; // cardinal
        else if (i > 3 && i < directions.size()) dirType = 1; // ordinal
        else dirType = 2; // secondary intercardinal
        float curX = xPos[i];
        std::string curDir = i > 7 ? "" : directions[i];

        for (int i = 0; i < colStrings.size(); i++) cols[i]->a = settings.getSettingByName<float>(colStrings[i] + "Opacity")->value;

        if (getOps<bool>("wrapFade")) {
            float distanceFromCenter = std::abs(curX - hudCenterX);
            float maxDistance = fullCirclePixelWidth / 2;
            float omittedDistance = maxDistance * getOps<float>("fadeDistancePerc") / 100;

            if (distanceFromCenter > omittedDistance) for (size_t i = 0; i < colStrings.size(); i++) cols[i]->a = settings.getSettingByName<float>(colStrings[i] + "Opacity")->value * (1.f - (distanceFromCenter - omittedDistance) / (maxDistance - omittedDistance));
        }

        if (getOps<bool>("showScales")) {
            if ((dirType == 0 && getOps<bool>("showCardinalScale")) ||
                (dirType == 1 && getOps<bool>("showOrdinalScale")) ||
                (dirType == 2)) {
                float scaleX = (curX - (dirType == 1 ? ordinalBarWidth : (dirType == 2 ? ordinalBarWidth / 1.5f : cardinalBarWidth)) / 2.f);

                if (getOps<bool>("scaleShadow")) FlarialGUI::RoundedRect(
                    floor(scaleX) + Constraints::RelativeConstraint(getOps<float>("scaleShadowOffset")) * uiscale,
                    realcenter.y + Constraints::RelativeConstraint(getOps<float>("scaleShadowOffset")) * uiscale,
                    dirType != 0 ? ordinalScaleShadowCol : cardinalScaleShadowCol,
                    dirType == 1 ? ordinalBarWidth : (dirType == 2 ? ordinalBarWidth / 1.5f : cardinalBarWidth),
                    dirType == 1 ? barHeight / 2.f : (dirType == 2 ? barHeight / 3.f : barHeight),
                    0, 0
                );

                FlarialGUI::RoundedRect(
                    floor(scaleX),
                    realcenter.y,
                    dirType != 0 ? ordinalScaleCol : cardinalScaleCol,
                    dirType == 1 ? ordinalBarWidth : (dirType == 2 ? ordinalBarWidth / 1.5f : cardinalBarWidth),
                    dirType == 1 ? barHeight / 2.f : (dirType == 2 ? barHeight / 3.f : barHeight),
                    0, 0
                );
            }
        }
    }

    if (getOps<bool>("showWaypoints")) {
        std::shared_ptr<Waypoints> waypoints = std::dynamic_pointer_cast<Waypoints>(ModuleManager::getModule("Waypoints"));
        if (waypoints) {
            for (const auto& [name, wp] : waypoints->WaypointList) {
                if (!waypoints->settings.getSettingByName<bool>("state-" + FlarialGUI::cached_to_string(wp.index))) continue;
                if (waypoints->settings.getSettingByName<std::string>("world-" + FlarialGUI::cached_to_string(wp.index))->value != SDK::clientInstance->getLocalPlayer()->getLevel()->getWorldFolderName()) continue;
                if (waypoints->settings.getSettingByName<std::string>("dimension-" + FlarialGUI::cached_to_string(wp.index))->value != SDK::clientInstance->getBlockSource()->getDimension()->getName()) continue;

                Vec3<float> waypointPos = waypoints->getPos(wp.index);
                Vec3<float> *playerPos = SDK::clientInstance->getLocalPlayer()->getPosition();

                float relativeYaw = getRelativeYaw(playerPos->x, playerPos->z, waypointPos.x, waypointPos.z, lerpYaw);

                float waypointX = hudCenterX + (relativeYaw * pixelsPerDegree);
                if (waypointX > hudCenterX + fullCirclePixelWidth / 2.0f) waypointX -= fullCirclePixelWidth;
                if (waypointX < hudCenterX - fullCirclePixelWidth / 2.0f) waypointX += fullCirclePixelWidth;

                float waypointHeight = Constraints::RelativeConstraint(0.05f * uiscale);
                float waypointWidth = Constraints::RelativeConstraint(0.005f * uiscale);

                D2D_COLOR_F waypointCol = D2D1::ColorF(D2D1::ColorF::Red);
                SettingType<std::string>* colSetting = waypoints->settings.getSettingByName<std::string>("color-" + FlarialGUI::cached_to_string(wp.index));
                if (colSetting) waypointCol = FlarialGUI::HexToColorF(colSetting->value);

                FlarialGUI::RoundedRect(
                    floor(waypointX - waypointWidth / 2.f),
                    realcenter.y,
                    waypointCol,
                    waypointWidth,
                    waypointHeight,
                    0, 0
                );
            }
        }
    }

    for (int i = 0; i < yawSize; i++) {
        int dirType;
        if (i < 4) dirType = 0; // cardinal
        else if (i > 3 && i < directions.size()) dirType = 1; // ordinal
        else dirType = 2; // secondary intercardinal
        float curX = xPos[i];
        std::string curDir = i > 7 ? "" : directions[i];

        for (int i = 0; i < colStrings.size(); i++) cols[i]->a = settings.getSettingByName<float>(colStrings[i] + "Opacity")->value;

        if (getOps<bool>("wrapFade")) {
            float distanceFromCenter = std::abs(curX - hudCenterX);
            float maxDistance = fullCirclePixelWidth / 2;
            float omittedDistance = maxDistance * getOps<float>("fadeDistancePerc") / 100;

            if (distanceFromCenter > omittedDistance) for (size_t i = 0; i < colStrings.size(); i++) cols[i]->a = settings.getSettingByName<float>(colStrings[i] + "Opacity")->value * (1.f - (distanceFromCenter - omittedDistance) / (maxDistance - omittedDistance));
        }

        if (getOps<bool>("showText") && curDir != "") {
            if ((dirType == 0 && getOps<bool>("showCardinalText")) ||
                (dirType == 1 && getOps<bool>("showOrdinalText"))) {
                ImVec2 textMetrics = FlarialGUI::getFlarialTextSize(
                    FlarialGUI::to_wide(curDir).c_str(),
                    0, 0,
                    DWRITE_TEXT_ALIGNMENT_CENTER, dirType == 1 ? OtextSize : CtextSize,
                    DWRITE_FONT_WEIGHT_NORMAL, true);

                if (getOps<bool>("textShadow")) FlarialGUI::FlarialTextWithFont(
                    curX + Constraints::RelativeConstraint(getOps<float>("textShadowOffset")) * uiscale,
                    realcenter.y + ((dirType == 1 ? barHeight / 2.f : barHeight) + textMetrics.y / 2.f) * (1.2f * uiscale) + Constraints::RelativeConstraint(getOps<float>("textShadowOffset")) * uiscale + (dirType == 1 ? ordinalTextOffset : cardinalTextOffset),
                    FlarialGUI::to_wide(curDir).c_str(),
                    0, 0,
                    DWRITE_TEXT_ALIGNMENT_CENTER, dirType == 1 ? OtextSize : CtextSize,
                    DWRITE_FONT_WEIGHT_NORMAL,
                    dirType == 1 ? ordinalTextShadowCol : cardinalTextShadowCol, true);

                FlarialGUI::FlarialTextWithFont(
                    curX,
                    realcenter.y + ((dirType == 1 ? barHeight / 2.f : barHeight) + textMetrics.y / 2.f) * (1.2f * uiscale) + (dirType == 1 ? ordinalTextOffset : cardinalTextOffset),
                    FlarialGUI::to_wide(curDir).c_str(),
                    0, 0,
                    DWRITE_TEXT_ALIGNMENT_CENTER, dirType == 1 ? OtextSize : CtextSize,
                    DWRITE_FONT_WEIGHT_NORMAL,
                    dirType == 1 ? ordinalTextCol : cardinalTextCol, true);
            }
        }
    }

}
