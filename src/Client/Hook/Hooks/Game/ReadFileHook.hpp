#pragma once

#include "../Hook.hpp"
#include "../../../../Utils/Memory/Game/SignatureAndOffsetManager.hpp"
#include "src/Utils/path.hpp"
#include <string>
#include <cstring>
#include "filesystem"
#include "src/Client/Events/Game/ReadFileEvent.hpp"

class ReadFileHook : public Hook {
private:
    static std::string *readFile(void *This, std::string *retstr, Core::Path &path) {
        std::string *result = funcOriginal(This, retstr, path);

        auto event = nes::make_holder<ReadFileEvent>(This, retstr, path, result);
        eventMgr.trigger(event);

        return event->result;
    }

public:
    typedef std::string *(__thiscall *original)(void *This, std::string *retstr, Core::Path &path);
    static inline original funcOriginal = nullptr;

    ReadFileHook() : Hook("ReadFileHook", GET_SIG_ADDRESS("AppPlatform::readAssetFile")) {}

    void enableHook() override {
        this->autoHook((void *)readFile, (void **)&funcOriginal);
    }
};
