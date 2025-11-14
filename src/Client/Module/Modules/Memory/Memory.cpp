#include "Memory.hpp"


void MEM::onEnable()
{
    Listen(this, RenderEvent, &MEM::onRender)
    Module::onEnable();
}

void MEM::onDisable()
{
    Deafen(this, RenderEvent, &MEM::onRender)
    Module::onDisable();
}

void MEM::defaultConfig()
{
    setDef("textscale", 0.8f);
    setDef("showMegabytes", false);
    Module::defaultConfig("all");
    
}

void MEM::settingsRender(float settingsOffset)
{
    initSettingsPage();

    defaultAddSettings("main");
    extraPadding();

    addHeader("Text");
    defaultAddSettings("text");
    extraPadding();

    addHeader("Module Settings");
    addToggle("Show Megabytes", "Display memory usage in MB instead of percentage", "showMegabytes");
    extraPadding();

    addHeader("Colors");
    defaultAddSettings("colors");
    extraPadding();

    addHeader("Misc");
    defaultAddSettings("misc");

    FlarialGUI::UnsetScrollView();
    resetPadding();
}

void MEM::onRender(RenderEvent& event)
{
    if (isEnabled()) {
        MEMORYSTATUSEX memory_status;
        memory_status.dwLength = sizeof(memory_status);
        GlobalMemoryStatusEx(&memory_status);
        DWORDLONG total_memory = memory_status.ullTotalPhys;
        DWORDLONG free_memory = memory_status.ullAvailPhys;
        DWORDLONG used_memory = total_memory - free_memory;

        std::string text;

        if (getOps<bool>("showMegabytes")) {
            // Convert bytes to megabytes
            DWORDLONG used_mb = used_memory / (1024 * 1024);
            DWORDLONG total_mb = total_memory / (1024 * 1024);
            text = FlarialGUI::cached_to_string(static_cast<int>(used_mb)) + " / " + 
                   FlarialGUI::cached_to_string(static_cast<int>(total_mb)) + " MB";
        } else {
            // Show percentage
            int sussymem = static_cast<int>((used_memory * 100) / total_memory);
            text = FlarialGUI::cached_to_string(sussymem) + "%";
        }

        this->normalRender(4, text);
    }
}
