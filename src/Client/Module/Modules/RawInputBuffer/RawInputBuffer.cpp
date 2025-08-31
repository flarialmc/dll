#include "RawInputBuffer.hpp"

RawInputBuffer::RawInputBuffer(): Module("Raw Input Buffer", "Fixes Minecraft's default input delay", IDR_CURSOR_PNG, "")
{
    
}

void RawInputBuffer::onEnable()
{
    Module::onEnable();
}

void RawInputBuffer::onDisable()
{
    Module::onDisable();
}

void RawInputBuffer::defaultConfig()
{
    Module::defaultConfig("core");
    
}

void RawInputBuffer::settingsRender(float settingsOffset)
{

    initSettingsPage();
    addHeader("troll");
    std::string troll("troll");
    addDropdown("troll", "the voices", std::vector<std::string>{ "troll", "troll", "troll even more", "most troll"}, troll);
    FlarialGUI::UnsetScrollView();

    resetPadding();
}
