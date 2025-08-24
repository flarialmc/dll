#include "Waila.hpp"
#include "Client.hpp"
#include "Events/EventManager.hpp"

void Waila::onEnable()
{
    Listen(this, SetupAndRenderEvent, &Waila::onSetupAndRender)
    Listen(this, RenderEvent, &Waila::onRender)
    Module::onEnable();
}

void Waila::onDisable()
{
    Deafen(this, SetupAndRenderEvent, &Waila::onSetupAndRender)
    Deafen(this, RenderEvent, &Waila::onRender)
    Module::onDisable();
}

void Waila::defaultConfig()
{
    setDef("responsivewidth", true);
    setDef("textscale", 0.80f);
    Module::defaultConfig("all");
    setDef("advanced", false);
    setDef("showAir", false);
    
}

void Waila::settingsRender(float settingsOffset)
{
    initSettingsPage();

    addHeader("Main");
    defaultAddSettings("main");
    addToggle("Advanced Mode", "", "advanced");
    addToggle("Show Air", "", "showAir");
    extraPadding();

    addHeader("Text");
    defaultAddSettings("text");
    extraPadding();

    addHeader("Colors");
    defaultAddSettings("colors");
    extraPadding();

    addHeader("Misc Customizations");
    defaultAddSettings("misc");

    FlarialGUI::UnsetScrollView();
    resetPadding();
}

void Waila::onSetupAndRender(SetupAndRenderEvent& event)
{
    if (!this->isEnabled()) return;
    if (!SDK::clientInstance->getLocalPlayer()) return;
    if (!SDK::clientInstance->getLocalPlayer()->getLevel()) return;
    if (!SDK::clientInstance->getBlockSource()) return;
    HitResult result = SDK::clientInstance->getLocalPlayer()->getLevel()->getHitResult();

    BlockPos pos = { result.blockPos.x,
        result.blockPos.y ,
        result.blockPos.z };
    BlockSource* blockSource = SDK::clientInstance->getBlockSource();
    try {
        BlockLegacy* block = blockSource->getBlock(pos)->getBlockLegacy();
        if (!block) return;
        try {

            if (!getOps<bool>("advanced")) lookingAt = block->getName();
            else lookingAt = block->getNamespace() + ":" + block->getName();
        }
        catch (const std::exception& e) { LOG_ERROR("Failed to get block name: {}", e.what()); }
    }
    catch (const std::exception& e) {
        LOG_ERROR("Failed to get block: {}", e.what());
    }
}

void Waila::onRender(RenderEvent& event)
{
    if (this->isEnabled()) {
        if ((lookingAt == "air" || lookingAt == "minecraft:air") && !getOps<bool>("showAir")) return;
        this->normalRender(32, lookingAt);
    }
}
