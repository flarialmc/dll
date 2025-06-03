#pragma once

#include <format>
#include "../../../Events/Listener.hpp"
#include "../../../Events/Input/KeyEvent.hpp"
#include "../Module.hpp"
#include "../../../GUI/Engine/Engine.hpp"
#include "../../../../SDK/SDK.hpp"
#include <Windows.h>
#include "../../../../SDK/Client/Actor/Gamemode.hpp"
#include "../../Manager.hpp"

class SwingListener : public Listener {

    Module *module;

    void* ogBytes[5] = { 0 };



    void onTick(TickEvent &event) override {

        std::vector<uint8_t> bytes = { 0x90, 0x90, 0x90, 0x90, 0x90 };

        Memory::patchBytes(SwingListener::fluxSwingAddr, bytes.data(), 5);
        *SwingListener::tapSwingVal = -module->getOps<float>("modifier");


        patchAnimFunc(false);

    }

public:
    explicit SwingListener(const char string[5], Module *module) {
        this->name = string;
        this->module = module;

    }
    static inline void *fluxSwingAddr = 0x0;
    static inline float *tapSwingVal = nullptr;
    static void patchAnimFunc(bool patch = true)
    {

        static void* addr = (void*) Memory::findSig(GET_SIG("ItemInHandRenderer::renderBp"));
        if (addr == 0) return;

        std::vector<uint8_t> bytes = {};

        if (patch) {
            bytes = { 0x90, 0x90, 0x90, 0x90 };
        }
        else {
            bytes = { 0xF3, 0x0F, 0x5C, 0xD0 };
        }

        Memory::patchBytes(addr, bytes.data(), bytes.size());
    }
};

