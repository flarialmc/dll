#include "GuiData.hpp"

#include "../../../Utils/Memory/Game/SignatureAndOffsetManager.hpp"
#include "../../../Utils/Versions/WinrtUtils.hpp"

void GuiData::displayClientMessage(const std::string &str) {
    if (str.empty())
        return;

    static uintptr_t sig;

    if (sig == NULL) {
        sig = Memory::findSig(GET_SIG("GuiData::displayClientMessage"));
    }

    if (WinrtUtils::check(21, 20)) {
        char bruh[0x28]{};

        using func_t = void(*)(GuiData*, const std::string&, char*, bool);
        static auto func = reinterpret_cast<func_t>(sig);
        func(this, str, bruh, true);
    }
    else {
        using func_t = void(*)(GuiData*, const std::string&, bool);
        static auto func = reinterpret_cast<func_t>(sig);
        func(this, str, true);
    }
}
