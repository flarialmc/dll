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
        using func_t = void(*)(GuiData*, const std::string&, std::optional<std::string>, bool); // The new optional seems to be some kind of filtered message for the future. If set, it overwrites the other string
        static auto func = reinterpret_cast<func_t>(sig);
        func(this, str, {}, true);
    }
    else {
        using func_t = void(*)(GuiData*, const std::string&, bool);
        static auto func = reinterpret_cast<func_t>(sig);
        func(this, str, true);
    }
}
