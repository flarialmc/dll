#include "RenderOptions.hpp"

#include "SDK/Client/Core/Options.hpp"


void RenderOptions::updateSetings()
{
    const auto showChunkMap = Options::getOption("dev_showChunkMap");
    const auto disableSky = Options::getOption("dev_disableRenderSky");
    const auto disableWeather = Options::getOption("dev_disableRenderWeather");
    const auto disableEntities = Options::getOption("dev_disableRenderEntities");
    const auto disableBlockEntities = Options::getOption("dev_disableRenderBlockEntities");
    const auto disableParticles = Options::getOption("dev_disableRenderParticles");
    const auto disableTerrain = Options::getOption("dev_disableRenderTerrain");
    // const auto disableMainMenuCubemap = Options::getOption("dev_disableRenderMainMenuCubemap");
    const auto disableItemInHand = Options::getOption("dev_disableRenderItemInHand");
    const auto disableHud = Options::getOption("dev_disableRenderHud");
    // const auto showLatencyGraph = Options::getOption("dev_showLatencyGraph");
    // const auto enableDebugHudOverlay = Options::getOption("dev_enableDebugHudOverlay");
    // const auto achievementsAlwaysEnabled = Options::getOption("dev_achievementsAlwaysEnabled");
    // const auto showDevConsoleButton = Options::getOption("dev_showDevConsoleButton");
    // const auto showBuildInfo = Options::getOption("dev_showBuildInfo");
    // const auto renderPaths = Options::getOption("dev_renderPaths");
    // const auto renderBoundingBox = Options::getOption("dev_renderBoundingBox");
    // const auto renderSchedulerData = Options::getOption("dev_renderSchedulerData");
    // const auto renderMobInfoState = Options::getOption("dev_renderMobInfoState");
    // const auto renderGoalState = Options::getOption("dev_renderGoalState");

    if (isEnabled()) {
        if (showChunkMap != nullptr) showChunkMap->setvalue(getOps<bool>("chunkborders"));
        if (disableSky != nullptr) disableSky->setvalue(!getOps<bool>("sky"));
        if (disableWeather != nullptr) disableWeather->setvalue(!getOps<bool>("weather"));
        if (disableEntities != nullptr) disableEntities->setvalue(!getOps<bool>("entity"));
        if (disableBlockEntities != nullptr) disableBlockEntities->setvalue(!getOps<bool>("blockentity"));
        if (disableParticles != nullptr) disableParticles->setvalue(!getOps<bool>("particles"));
        if (disableTerrain != nullptr) disableTerrain->setvalue(!getOps<bool>("terrain"));
        // if (disableMainMenuCubemap != nullptr) disableMainMenuCubemap->setvalue(!getOps<bool>("mainmenucubemap"));
        if (disableItemInHand != nullptr) disableItemInHand->setvalue(!getOps<bool>("iteminhand"));
        if (disableHud != nullptr) disableHud->setvalue(!getOps<bool>("hud"));
        // if (showLatencyGraph != nullptr) showLatencyGraph->setvalue(getOps<bool>("latencygraph"));
        // if (enableDebugHudOverlay != nullptr) enableDebugHudOverlay->setvalue(getOps<bool>("debughudoverlay"));
        // if (achievementsAlwaysEnabled != nullptr) achievementsAlwaysEnabled->setvalue(getOps<bool>("achievementsalwaysenabled"));
        // if (showDevConsoleButton != nullptr) showDevConsoleButton->setvalue(getOps<bool>("showdevconsolebutton"));
        // if (showBuildInfo != nullptr) showBuildInfo->setvalue(getOps<bool>("showbuildinfo"));
        // if (renderPaths != nullptr) renderPaths->setvalue(getOps<bool>("renderpaths"));
        // if (renderBoundingBox != nullptr) renderBoundingBox->setvalue(getOps<bool>("renderboundingbox"));
        // if (renderSchedulerData != nullptr) renderSchedulerData->setvalue(getOps<bool>("renderschedulerdata"));
        // if (renderMobInfoState != nullptr) renderMobInfoState->setvalue(getOps<bool>("rendermobinfostate"));
        // if (renderGoalState != nullptr) renderGoalState->setvalue(getOps<bool>("rendergoalstate"));
    }
    else {
        if (showChunkMap != nullptr) showChunkMap->setvalue(false);
        if (disableSky != nullptr) disableSky->setvalue(false);
        if (disableWeather != nullptr) disableWeather->setvalue(false);
        if (disableEntities != nullptr) disableEntities->setvalue(false);
        if (disableBlockEntities != nullptr) disableBlockEntities->setvalue(false);
        if (disableParticles != nullptr) disableParticles->setvalue(false);
        if (disableTerrain != nullptr) disableTerrain->setvalue(false);
        // if (disableMainMenuCubemap != nullptr) disableMainMenuCubemap->setvalue(false);
        if (disableItemInHand != nullptr) disableItemInHand->setvalue(false);
        if (disableHud != nullptr) disableHud->setvalue(false);
        // if (showLatencyGraph != nullptr) showLatencyGraph->setvalue(false);
        // if (enableDebugHudOverlay != nullptr) enableDebugHudOverlay->setvalue(false);
        // if (achievementsAlwaysEnabled != nullptr) achievementsAlwaysEnabled->setvalue(false);
        // if (showDevConsoleButton != nullptr) showDevConsoleButton->setvalue(false);
        // if (showBuildInfo != nullptr) showBuildInfo->setvalue(false);
        // if (renderPaths != nullptr) renderPaths->setvalue(false);
        // if (renderBoundingBox != nullptr) renderBoundingBox->setvalue(false);
        // if (renderSchedulerData != nullptr) renderSchedulerData->setvalue(false);
        // if (renderMobInfoState != nullptr) renderMobInfoState->setvalue(false);
        // if (renderGoalState != nullptr) renderGoalState->setvalue(false);
    }
}

void RenderOptions::onEnable()
{
    Listen(this, SetupAndRenderEvent, &RenderOptions::onSetupAndRender)
    Module::onEnable();
    updateSetings();
}

void RenderOptions::onDisable()
{
    Deafen(this, SetupAndRenderEvent, &RenderOptions::onSetupAndRender)
    Module::onDisable();
    updateSetings();
}

void RenderOptions::defaultConfig()
{
    Module::defaultConfig("core");
    setDef("chunkborders", false);
    setDef("sky", true);
    setDef("weather", true);
    setDef("entity", true);
    setDef("blockentity", true);
    setDef("particles", true);
    setDef("terrain", true);
    // setDef("mainmenucubemap", true);
    setDef("iteminhand", true);
    setDef("hud", true);
    // setDef("latencygraph", false);
    // setDef("debughudoverlay", false);
    // setDef("achievementsalwaysenabled", false);
    // setDef("showdevconsolebutton", false);
    // setDef("showbuildinfo", false);
    // setDef("renderpaths", false);
    // setDef("renderboundingbox", false);
    // setDef("renderschedulerdata", false);
    // setDef("rendermobinfostate", false);
    // setDef("rendergoalstate", false);
    
}

void RenderOptions::settingsRender(float settingsOffset)
{

    updateSetings();

    initSettingsPage();

    addToggle("Chunk Borders", "Shifts block around so you can see the chunk borders.", "chunkborders");
    addToggle("Render Sky", "Look up and see the difference.", "sky");
    addToggle("Render Entities", "Animals, monsters, items and more.", "entity");
    addToggle("Render Block Entities", "Chests, shulker boxes, etc.", "blockentity");
    addToggle("Render Particles", "", "particles");
    addToggle("Render Terrain", "Not worth disabling.", "terrain");
    addToggle("Render Weather", "", "weather");
    // addToggle("Render Main Menu Cubemap", "", "mainmenucubemap");
    addToggle("Render Item In Hand", "Doesn't work correctly.", "iteminhand");
    addToggle("Render HUD", "F1 but bad. Toggle HUD on and off with F1 to see it not coming back.", "hud");
    // addToggle("Show Latency Graph", "", "latencygraph");
    // addToggle("Enable Debug HUD Overlay", "", "debughudoverlay");
    // addToggle("Achievements Always Enabled", "", "achievementsalwaysenabled");
    // addToggle("Show Dev Console Button", "", "showdevconsolebutton");
    // addToggle("Show Build Info", "", "showbuildinfo");
    // addToggle("Render Paths", "", "renderpaths");
    // addToggle("Render Bounding Box", "", "renderboundingbox");
    // addToggle("Render Scheduler Data", "", "renderschedulerdata");
    // addToggle("Render Mob Info State", "", "rendermobinfostate");
    // addToggle("Render Goal State", "", "rendergoalstate");

    FlarialGUI::UnsetScrollView();
    resetPadding();
}

void RenderOptions::onSetupAndRender(SetupAndRenderEvent& event)
{
    if (!this->isEnabled()) return;
    if (Options::isInitialized()) {
        updateSetings();
    };
}
