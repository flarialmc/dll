#include "RawInputBuffer.hpp"


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
    // TODO: placeholder settings - replace with actual configuration options
    addHeader("Settings");
    std::string placeholder("placeholder");
    addDropdown("placeholder", "Option", std::vector<std::string>{ "Option 1", "Option 2", "Option 3", "Option 4"}, placeholder);
    FlarialGUI::UnsetScrollView();

    resetPadding();
}
