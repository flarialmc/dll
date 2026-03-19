#pragma once

#include "../Hook.hpp"
#include "../../../../Utils/Memory/Game/SignatureAndOffsetManager.hpp"
#include "../../../Events/Input/SensitivityEvent.hpp"
#include "SDK/Client/Core/ClientInstance.hpp"
#include "Module/Manager.hpp"
#include "Client.hpp"
#include "Modules/Nick/NickModule.hpp"
#include "Utils/APIUtils.hpp"
#include <filesystem>
#include <fstream>
#include <ranges>

class ClientInstanceUpdateHook : public Hook {
private:
    static void Update(ClientInstance* client, bool a2) {
        return funcOriginal(client, a2);
    }

public:
    typedef void(__thiscall* original)(ClientInstance*, bool);

    static inline original funcOriginal = nullptr;

    ClientInstanceUpdateHook() : Hook("ClientInstanceUpdateHook", GET_SIG_ADDRESS("ClientInstance::update")) {}

    void enableHook() override {
        this->autoHook((void*)Update, (void**)&funcOriginal);
    }
};