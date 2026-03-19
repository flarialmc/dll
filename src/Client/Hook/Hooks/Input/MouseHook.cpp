#include "MouseHook.hpp"
#include "../../../Client.hpp"
#include "../../../../Utils/Memory/Game/SignatureAndOffsetManager.hpp"
#include "../../../Module/Modules/ClickGUI/ClickGUI.hpp"
#include "Utils/PlatformUtils.hpp"

MouseHook::MouseHook() : Hook("mouse_hook", Memory::offsetFromSig(GET_SIG_ADDRESS("MouseDevice::feed"), 1)) {
}

void MouseHook::enableHook() {
    if (VersionUtils::checkAboveOrEqual(21, 120))
    {
        this->manualHook((void*)Memory::offsetFromSig(GET_SIG_ADDRESS("InputHandler::tick"), 1), (void *) InputHandler_tick, (void **) &func0);
    }
    else
    {
        this->autoHook((void *) mouseCallback, (void **) &funcOriginal);
    }

}

void MouseHook::mouseCallback(void *mouseDevice, char button, char action, short mouseX, short mouseY,
                              short movementX,
                              short movementY, char a8) {

    // eventemitter here

    // BUTTON
    // 0 -> Mouse move, mouseX,mouseY
    // 4 -> Mouse wheel, state/WHEEL_DELTA
    // rest -> Mouse button, state

    // parm_1, parm_8 (might be isScrolling?) -> ???
    auto event = nes::make_holder<MouseEvent>(button, action, mouseX, mouseY, movementX, movementY);
    eventMgr.trigger(event);

    if (!event->isCancelled()) {
        return funcOriginal(mouseDevice, event->getButton(), event->getActionAsChar(), mouseX, mouseY, movementX,
                            movementY, a8);
    }
}

struct MouseInputPacket {
    int16_t x;
    int16_t y;

private:
    int pad;

public:
    int8_t type;
    int8_t state;

private:
    char pad2[0x8];
};

void* MouseHook::InputHandler_tick(void *_this, void *a2, void *a3, void *a4) {
    if (Client::disable) return func0(_this, a2, a3, a4);

    static auto mouseInputVector = Memory::getOffsetFromSig<std::vector<MouseInputPacket>*>(GET_SIG_ADDRESS("MouseInputVector"), 3);

    if (!mouseInputVector)
        return func0(_this, a2, a3, a4);

    static int LastX = 0;
    static int LastY = 0;
    static bool wasGuiOpen = false;

    for (auto it = mouseInputVector->begin(); it != mouseInputVector->end(); ) {
        const auto& packet = *it;

        auto event = nes::make_holder<MouseEvent>(
                packet.type, packet.state,
                packet.x, packet.y,
                LastX, LastY
        );

        eventMgr.trigger(event);
        LastX = packet.x;
        LastY = packet.y;

        if (event->isCancelled()) {
            it = mouseInputVector->erase(it);
        } else {
            ++it;
        }
    }

    // When ClickGUI just opened, inject button release packets so the game
    // stops any held actions (block breaking, attacking, etc.). These are
    // added AFTER event processing so they won't be cancelled by ClickGUI's
    // onMouse handler — they go straight to the game via func0.
    bool guiOpen = ClickGUI::menuOpen || ClickGUI::editmenu;
    if (guiOpen && !wasGuiOpen) {
        MouseInputPacket releaseLeft{};
        releaseLeft.type = static_cast<int8_t>(MouseButton::Left);
        releaseLeft.state = 0; // Release (raw char value)
        mouseInputVector->push_back(releaseLeft);

        MouseInputPacket releaseRight{};
        releaseRight.type = static_cast<int8_t>(MouseButton::Right);
        releaseRight.state = 0;
        mouseInputVector->push_back(releaseRight);
    }
    wasGuiOpen = guiOpen;

    return func0(_this, a2, a3, a4);
}
