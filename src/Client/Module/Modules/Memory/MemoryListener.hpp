#pragma once

#include "../../../../SDK/SDK.hpp"
#include <format>
#include "../../../Events/Listener.hpp"
#include "../../../Events/Input/KeyEvent.hpp"
#include "../Module.hpp"
#include "../../../GUI/Engine/Engine.hpp"
#include <Windows.h>

class MemoryListener : public Listener {

    Module* module;

    void onRender(RenderEvent& event) override {

        if(SDK::CurrentScreen == "hud_screen")
        if (module->settings.getSettingByName<bool>("enabled")->value) {


            MEMORYSTATUSEX memory_status;
            memory_status.dwLength = sizeof(memory_status);
            GlobalMemoryStatusEx(&memory_status);
            DWORDLONG total_memory = memory_status.ullTotalPhys;
            DWORDLONG free_memory = memory_status.ullAvailPhys;
            DWORDLONG used_memory = total_memory - free_memory;

            int sussymem = static_cast<int>((used_memory * 100) / total_memory);

            this->module->NormalRender(4, module->settings.getSettingByName<std::string>("text")->value, std::to_string(sussymem) + "%");

        }
    }

public:
    explicit MemoryListener(const char string[5], Module* module) {
        this->name = string;
        this->module = module;
    }

};

