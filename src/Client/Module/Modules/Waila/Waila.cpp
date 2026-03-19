#include "Waila.hpp"
#include "Client.hpp"

void Waila::onEnable()
{
    HUDModule::onEnable();
    Listen(this, SetupAndRenderEvent, &Waila::onSetupAndRender)
}

void Waila::onDisable()
{
    Deafen(this, SetupAndRenderEvent, &Waila::onSetupAndRender)
    HUDModule::onDisable();
}

void Waila::customConfig()
{
    setDef("responsivewidth", true);
    setDef("textscale", 0.80f);
    setDef("advanced", false);
    setDef("showAir", false);
}

void Waila::customSettings()
{
    addToggle("Advanced Mode", "", "advanced");
    addToggle("Show Air", "", "showAir");
}

std::string Waila::getDisplayValue()
{
    if ((lookingAt == "air" || lookingAt == "minecraft:air") && !getOps<bool>("showAir")) return "";
    return lookingAt;
}

void Waila::onSetupAndRender(SetupAndRenderEvent& event)
{
    if (!this->isEnabled()) return;
    if (!SDK::clientInstance || !SDK::clientInstance->getLocalPlayer()) return;
    if (!SDK::clientInstance->getLocalPlayer()->getLevel()) return;
    if (!SDK::clientInstance->getBlockSource()) return;
    HitResult result = SDK::clientInstance->getLocalPlayer()->getLevel()->getHitResult();

    BlockPos pos = { result.blockPos.x,
        result.blockPos.y ,
        result.blockPos.z };
    BlockSource* blockSource = SDK::clientInstance->getBlockSource();
    try {
        Block* oBlock = blockSource->getBlock(pos);
        BlockLegacy* block = oBlock->getBlockLegacy();
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
