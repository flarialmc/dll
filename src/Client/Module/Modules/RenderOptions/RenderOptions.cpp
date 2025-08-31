#include "RenderOptions.hpp"

#include "SDK/Client/Core/Options.hpp"

RenderOptions::RenderOptions(): Module("Render Option", "Change the way how the game is rendered.",
                                       IDR_RENDEROPTIONS_PNG, "")
{

    
}

void RenderOptions::updateSetings()
{
    auto showChunkMap = Options::getOption("dev_showChunkMap");
    auto disableSky = Options::getOption("dev_disableRenderSky");
    auto disableWeather = Options::getOption("dev_disableRenderWeather");
    auto disableEntities = Options::getOption("dev_disableRenderEntities");
    auto disableBlockEntities = Options::getOption("dev_disableRenderBlockEntities");
    auto disableParticles = Options::getOption("dev_renderBoundingBox");

    if (isEnabled()) {
        if (showChunkMap != nullptr) showChunkMap->setvalue(getOps<bool>("chunkborders"));
        if (disableSky != nullptr) disableSky->setvalue(!getOps<bool>("sky"));
        if (disableWeather != nullptr) disableWeather->setvalue(!getOps<bool>("weather"));
        if (disableEntities != nullptr) disableEntities->setvalue(!getOps<bool>("entity"));
        if (disableBlockEntities != nullptr) disableBlockEntities->setvalue(!getOps<bool>("blockentity"));
        if (disableParticles != nullptr) disableParticles->setvalue(!getOps<bool>("particles"));
    }
    else {
        if (showChunkMap != nullptr) showChunkMap->setvalue(false);
        if (disableSky != nullptr) disableSky->setvalue(false);
        if (disableWeather != nullptr) disableWeather->setvalue(false);
        if (disableEntities != nullptr) disableEntities->setvalue(false);
        if (disableBlockEntities != nullptr) disableBlockEntities->setvalue(false);
        if (disableParticles != nullptr) disableParticles->setvalue(false);
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
    
}

void RenderOptions::settingsRender(float settingsOffset)
{

    updateSetings();

    initSettingsPage();

    addHeader("Render Options");
    addToggle("Chunk Borders", "", "chunkborders");
    addToggle("Render Sky", "", "sky");
    addToggle("Render Entities", "", "entity");
    addToggle("Render Block Entities", "", "blockentity");
    addToggle("Render Particles", "", "particles");
    addToggle("Render Weather", "", "weather");

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
