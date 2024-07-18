#pragma once

#include "../../../../SDK/SDK.hpp"
#include <format>
#include "../../../Events/Listener.hpp"
#include "../../../Events/Input/KeyEvent.hpp"
#include "../Module.hpp"

#include <Windows.h>

class MemoryListener : public Listener {

    Module *module;

    void onRender(RenderEvent &event) override {
        if (module->isEnabled()) {
            //TODO: (Memory module) Do megabytes mode
            MEMORYSTATUSEX memory_status;
            memory_status.dwLength = sizeof(memory_status);
            GlobalMemoryStatusEx(&memory_status);
            DWORDLONG total_memory = memory_status.ullTotalPhys;
            DWORDLONG free_memory = memory_status.ullAvailPhys;
            DWORDLONG used_memory = total_memory - free_memory;

            int sussymem = static_cast<int>((used_memory * 100) / total_memory);

            std::string text = std::to_string(sussymem) + "%";

            this->module->normalRender(4, text);

        }
    }

public:
    explicit MemoryListener(const char string[5], Module *module) {
        this->name = string;
        this->module = module;
    }

};

