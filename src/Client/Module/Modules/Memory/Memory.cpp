#include "Memory.hpp"

MEM::MEM(): Module("Memory", "Shows your current system RAM usage.", IDR_MEMORY_PNG, "")
{

    
}

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
    Module::defaultConfig("all");
    
}

void MEM::settingsRender(float settingsOffset)
{
    float x = Constraints::PercentageConstraint(0.019, "left");
    float y = Constraints::PercentageConstraint(0.10, "top");

    const float scrollviewWidth = Constraints::RelativeConstraint(0.5, "height", true);


    FlarialGUI::ScrollBar(x, y, 140, Constraints::SpacingConstraint(5.5, scrollviewWidth), 2);
    FlarialGUI::SetScrollView(x - settingsOffset, Constraints::PercentageConstraint(0.00, "top"),
                              Constraints::RelativeConstraint(1.0, "width"),
                              Constraints::RelativeConstraint(0.88f, "height"));

    addHeader("Memory");
    defaultAddSettings("main");
    extraPadding();

    addHeader("Text");
    defaultAddSettings("text");
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
        //TODO: (Memory module) Do megabytes mode
        MEMORYSTATUSEX memory_status;
        memory_status.dwLength = sizeof(memory_status);
        GlobalMemoryStatusEx(&memory_status);
        DWORDLONG total_memory = memory_status.ullTotalPhys;
        DWORDLONG free_memory = memory_status.ullAvailPhys;
        DWORDLONG used_memory = total_memory - free_memory;

        int sussymem = static_cast<int>((used_memory * 100) / total_memory);

        std::string text = FlarialGUI::cached_to_string(sussymem) + "%";

        this->normalRender(4, text);
    }
}
