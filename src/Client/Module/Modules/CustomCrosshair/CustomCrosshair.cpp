#include "CustomCrosshair.hpp"
#include "Client.hpp"
#include "Events/EventManager.hpp"
#include "Utils/Render/PositionUtils.hpp"

void CustomCrosshair::onEnable() {
    Listen(this, PerspectiveEvent, &CustomCrosshair::onGetViewPerspective)
    Listen(this, HudCursorRendererRenderEvent, &CustomCrosshair::onHudCursorRendererRender)
    Listen(this, RenderEvent, &CustomCrosshair::onRender)
    Module::onEnable();

    std::string Path = Utils::getClientPath() + "\\Crosshairs";

    for (const auto& entry : std::filesystem::directory_iterator(Path))
    {
        auto ch = new CrosshairImage(entry.path().string());
        Logger::debug("Crosshair: " + entry.path().string());

        std::string name = entry.path().filename().string();
        crosshairs[name.substr(0, name.size()-4)] = ch;

        Logger::debug("Crosshair loaded: " + name);

    }

    std::cout << crosshairs.empty() << std::endl;

    if (crosshairs.empty()) crosshairs["Crosshair1"] = new CrosshairImage();
}

void CustomCrosshair::onDisable() {
    Deafen(this, PerspectiveEvent, &CustomCrosshair::onGetViewPerspective)
    Deafen(this, HudCursorRendererRenderEvent, &CustomCrosshair::onHudCursorRendererRender)
    Deafen(this, RenderEvent, &CustomCrosshair::onRender)
    Module::onDisable();
}

void CustomCrosshair::defaultConfig() {
    settings.renameSetting("defaultColor", "defaultOpacity", "defaultColorRGB", "default");
    settings.renameSetting("enemyColor", "enemyOpacity", "enemyColorRGB", "enemy");
    setDef("uiscale", 1.f);
    Module::defaultConfig("core");
    setDef("CustomCrosshair", false);
    setDef("highlightOnEntity", false);
    setDef("solidColorWhenHighlighted", true);
    setDef("solidColor", false);
    setDef("renderInThirdPerson", false);
    setDef("default", (std::string) "fafafa", 0.55f, false);
    setDef("enemy", (std::string) "FF0000", 1.f, false);
    setDef("CurrentSelectedColor", (std::string)"FFFFFF", 1.f, false);
    setDef("CurrentCrosshair", (std::string)"Crosshair1");
    setDef("ShowGridLines", true);
    setDef("HighlightMiddleLine", true);

    getOps<std::string>("CurrentSelectedColorCol") = "FFFFFF";
    getOps<float>("CurrentSelectedColorOpacity") = 1.f;
    
}

void CustomCrosshair::settingsRender(float settingsOffset) {
    actuallyRenderWindow = true;
    auto button = MC::mouseButton;
    if (blankWindow)
        MC::mouseButton = MouseButton::None;

    initSettingsPage();


        addHeader("Crosshair Editor");
        addButton("Crosshair Editor", "Opens the crosshair editor menu", "open", [&]() {
            blankWindow = !blankWindow;
        });
        addButton("Reload Crosshair", "Reloads Crosshair to apply any changes", "reload", [&]() {
            CrosshairReloaded = true;
        });

        addToggle("Show Grid Lines", "Shows the grid of the canvas.", "ShowGridLines");
        addToggle("Highlight Middle Line", "Adds 2 red lines to show the middle of the canvas", "HighlightMiddleLine");

        extraPadding();


    addHeader("Main");
    addToggle("Use Custom Crosshair", "Uses a custom crosshair.","CustomCrosshair");
    addToggle("Solid Color", "Make crosshair a solid color / more visible", "solidColor");
    addToggle("Render in Third Person", "Weather or not to render in third person", "renderInThirdPerson");
    addToggle("Highlight on Entity", "Highlight when enemy in reach", "highlightOnEntity");
    addConditionalToggle(getOps<bool>("highlightOnEntity"), "Solid Color When Highlighted", "Use solid color when highlighted", "solidColorWhenHighlighted");

    extraPadding();

    addHeader("Misc");
    addSlider("Crosshair Scale", "The size of the Crosshair (only for custom)", "uiscale", 10.f, 0.f, true);

    extraPadding();

    addHeader("Colors");
    addColorPicker("Default Color", "When the enemy is not in view.", "default");
    addConditionalColorPicker(getOps<bool>("highlightOnEntity"), "Enemy Color", "When the enemy is in view.", "enemy");


    FlarialGUI::UnsetScrollView();

    resetPadding();

    if (blankWindow) MC::mouseButton = button;
}

void CustomCrosshair::onGetViewPerspective(PerspectiveEvent &event) {
    if (!this->isEnabled()) return;
    currentPerspective = event.getPerspective();
}

void CustomCrosshair::onHudCursorRendererRender(HudCursorRendererRenderEvent &event) {
    if (!this->isEnabled()) return;
    if (!SDK::clientInstance) return;
    auto player = SDK::clientInstance->getLocalPlayer();
    if (!player) return;
    if (SDK::getCurrentScreen() != "hud_screen") return;

    auto renderInThirdPerson = settings.getSettingByName<bool>("renderInThirdPerson")->value;
    if (!renderInThirdPerson && currentPerspective != Perspective::FirstPerson) return;
    bool isHoveringEnemy = (player->getLevel()->getHitResult().type == HitResultType::Entity);

    bool isDefault = !settings.getSettingByName<bool>("CustomCrosshair")->value;

    auto screenContext = event.getMinecraftUIRenderContext()->getScreenContext();

    const ResourceLocation loc("textures/ui/cross_hair", false);
    TexturePtr ptr = SDK::clientInstance->getMinecraftGame()->textureGroup->getTexture(loc, false);
    if (ptr.clientTexture == nullptr || ptr.clientTexture->clientTexture.resourcePointerBlock == nullptr) {
        return;
    }

    TexturePtr ptr2;

    if (std::filesystem::exists(
        Utils::getClientPath() + "//Crosshairs//" + settings.getSettingByName<std::string>("CurrentCrosshair")->value +
        ".png")) {
        const ResourceLocation loc2(
            Utils::getClientPath() + "//Crosshairs//" + settings.getSettingByName<std::string>("CurrentCrosshair")->
            value + ".png", true);
        ptr2 = SDK::clientInstance->getMinecraftGame()->textureGroup->getTexture(loc2, CrosshairReloaded);
        CrosshairReloaded = false;
        if (ptr2.clientTexture == nullptr || ptr2.clientTexture->clientTexture.resourcePointerBlock == nullptr) {
            isDefault = true;
        }
    } else {
        isDefault = true;
    }

    const auto tess = screenContext->getTessellator();

    tess->begin(mce::PrimitiveMode::QuadList, 4);

    D2D1_COLOR_F color = isHoveringEnemy && getOps<bool>("highlightOnEntity") ? getColor("enemy") : getColor("default");

    tess->color(color.r, color.g, color.b, color.a);

    Vec2<float> size = Vec2<float>(16, 16);
    auto scale = settings.getSettingByName<float>("uiscale")->value;

    Vec2<float> sizeUnscaled = PositionUtils::getScreenScaledPos(size);

    Vec2<float> sizeScaled = sizeUnscaled.mul(scale);

    auto SizeToUse = isDefault ? sizeUnscaled : sizeScaled;

    Vec2<float> pos = PositionUtils::getScaledPos(Vec2<float>((MC::windowSize.x / 2) - (SizeToUse.x / 2),
                                                              (MC::windowSize.y / 2) - (SizeToUse.y / 2)));

    auto useSolidColor = settings.getSettingByName<bool>("solidColor")->value;
    auto useSolidColorWhenHighlighted = settings.getSettingByName<bool>("solidColorWhenHighlighted")->value;

    bool useSolid = false;

    if (isHoveringEnemy && getOps<bool>("highlightOnEntity") && useSolidColorWhenHighlighted || (!isHoveringEnemy && useSolidColor)) {
        useSolid = true;
    }

    mce::MaterialPtr *material = useSolid ? MaterialUtils::getUITextured() : MaterialUtils::getUICrosshair();

    if (isDefault) {
        // Pack crosshairs have textures placed whereever so this will figure it out
        IntRectangle rect = IntRectangle(pos.x, pos.y, size.x, size.y);
        ScreenRenderer::blit(screenContext, &ptr, &rect, material);
    } else {
        size = size.mul(scale);
        tess->vertexUV(pos.x, pos.y + size.y, 0.f, 0.f, 1.f);
        tess->vertexUV(pos.x + size.x, pos.y + size.y, 0.f, 1.f, 1.f);
        tess->vertexUV(pos.x + size.x, pos.y, 0.f, 1.f, 0.f);
        tess->vertexUV(pos.x, pos.y, 0.f, 0.f, 0.f);
        MeshHelpers::renderMeshImmediately2(screenContext, tess, material, *ptr2.clientTexture);
    }

    event.cancel();
}

void CustomCrosshair::onRender(RenderEvent &event) {
    if (!this->isEnabled()) return;
    if (actuallyRenderWindow)
        CrosshairEditorWindow();
    else {
        blankWindow = false;
    }
    actuallyRenderWindow = false;
    if (!blankWindow) CurrentSelectedCrosshair = settings.getSettingByName<std::string>("CurrentCrosshair")->value;
}
