#include "GuiData.hpp"
#include "../../../Utils/Memory/Game/SignatureAndOffsetManager.hpp"

void GuiData::displayClientMessage(std::string &str) {
    if (str.empty())
        return;

    static uintptr_t sig;

    if (sig == NULL) {
        sig = Memory::findSig(GET_SIG("GuiData::displayClientMessage"));
    }

    auto fn = reinterpret_cast<void (__thiscall *)(GuiData*, const std::string&, bool)>(sig);
    return fn(this, str, false);
}
