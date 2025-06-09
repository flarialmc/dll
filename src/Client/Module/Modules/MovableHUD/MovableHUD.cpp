#include "MovableHUD.hpp"

MovableHUD::MovableHUD(): Module("Movable HUD", "Makes everything on screen movable!", IDR_MOVABLE_PNG, "")
{
    
}

void MovableHUD::onEnable()
{
}

void MovableHUD::onDisable()
{
}

void MovableHUD::defaultConfig()
{
    Module::defaultConfig("core");
    Module::defaultConfig("pos");
    if (ModuleManager::initialized) Client::SaveSettings();
}

void MovableHUD::settingsRender(float settingsOffset)
{}
