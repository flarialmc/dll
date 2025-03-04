#include <format>
#include "../../../../Events/Input/KeyEvent.hpp"
#include "../../../../../Utils/Logger/Logger.hpp"
#include "../../../../Client.hpp"
#include <Windows.h>

class ImGUIKeyListener : public Listener {
public:


    int ImGuiKeyToUnicode(ImGuiKey key) {
        switch (key) {
            case ImGuiKey_A: return 0x41; // Unicode for 'A'
            case ImGuiKey_B: return 0x42; // Unicode for 'B'
            case ImGuiKey_C: return 0x43; // Unicode for 'C'
            case ImGuiKey_D: return 0x44; // Unicode for 'D'
            case ImGuiKey_E: return 0x45; // Unicode for 'E'
            case ImGuiKey_F: return 0x46; // Unicode for 'F'
            case ImGuiKey_G: return 0x47; // Unicode for 'G'
            case ImGuiKey_H: return 0x48; // Unicode for 'H'
            case ImGuiKey_I: return 0x49; // Unicode for 'I'
            case ImGuiKey_J: return 0x4A; // Unicode for 'J'
            case ImGuiKey_K: return 0x4B; // Unicode for 'K'
            case ImGuiKey_L: return 0x4C; // Unicode for 'L'
            case ImGuiKey_M: return 0x4D; // Unicode for 'M'
            case ImGuiKey_N: return 0x4E; // Unicode for 'N'
            case ImGuiKey_O: return 0x4F; // Unicode for 'O'
            case ImGuiKey_P: return 0x50; // Unicode for 'P'
            case ImGuiKey_Q: return 0x51; // Unicode for 'Q'
            case ImGuiKey_R: return 0x52; // Unicode for 'R'
            case ImGuiKey_S: return 0x53; // Unicode for 'S'
            case ImGuiKey_T: return 0x54; // Unicode for 'T'
            case ImGuiKey_U: return 0x55; // Unicode for 'U'
            case ImGuiKey_V: return 0x56; // Unicode for 'V'
            case ImGuiKey_W: return 0x57; // Unicode for 'W'
            case ImGuiKey_X: return 0x58; // Unicode for 'X'
            case ImGuiKey_Y: return 0x59; // Unicode for 'Y'
            case ImGuiKey_Z: return 0x5A; // Unicode for 'Z'
            case ImGuiKey_0: return 0x30; // Unicode for '0'
            case ImGuiKey_1: return 0x31; // Unicode for '1'
            case ImGuiKey_2: return 0x32; // Unicode for '2'
            case ImGuiKey_3: return 0x33; // Unicode for '3'
            case ImGuiKey_4: return 0x34; // Unicode for '4'
            case ImGuiKey_5: return 0x35; // Unicode for '5'
            case ImGuiKey_6: return 0x36; // Unicode for '6'
            case ImGuiKey_7: return 0x37; // Unicode for '7'
            case ImGuiKey_8: return 0x38; // Unicode for '8'
            case ImGuiKey_9: return 0x39; // Unicode for '9'
            case ImGuiKey_Space: return 0x20;
            case ImGuiKey_Enter: return 0x0D;
            default: return -1;
        }
    }

    ImGuiKey keycodeToImGuiKey(int keycode)
    {
        switch (keycode)
        {
            case 9: return ImGuiKey_Tab;
            case 37: return ImGuiKey_LeftArrow;
            case 39: return ImGuiKey_RightArrow;
            case 38: return ImGuiKey_UpArrow;
            case 40: return ImGuiKey_DownArrow;
            case 33: return ImGuiKey_PageUp;
            case 34: return ImGuiKey_PageDown;
            case 36: return ImGuiKey_Home;
            case 35: return ImGuiKey_End;
            case 45: return ImGuiKey_Insert;
            case 46: return ImGuiKey_Delete;
            case 8: return ImGuiKey_Backspace;
            case 32: return ImGuiKey_Space;
            case 13: return ImGuiKey_Enter;
            case 27: return ImGuiKey_Escape;
            case 222: return ImGuiKey_Apostrophe;
            case 188: return ImGuiKey_Comma;
            case 189: return ImGuiKey_Minus;
            case 190: return ImGuiKey_Period;
            case 191: return ImGuiKey_Slash;
            case 186: return ImGuiKey_Semicolon;
            case 187: return ImGuiKey_Equal;
            case 219: return ImGuiKey_LeftBracket;
            case 220: return ImGuiKey_Backslash;
            case 221: return ImGuiKey_RightBracket;
            case 192: return ImGuiKey_GraveAccent;
            case 19: return ImGuiKey_Pause;
            case 96: return ImGuiKey_Keypad0;
            case 97: return ImGuiKey_Keypad1;
            case 98: return ImGuiKey_Keypad2;
            case 99: return ImGuiKey_Keypad3;
            case 100: return ImGuiKey_Keypad4;
            case 101: return ImGuiKey_Keypad5;
            case 102: return ImGuiKey_Keypad6;
            case 103: return ImGuiKey_Keypad7;
            case 104: return ImGuiKey_Keypad8;
            case 105: return ImGuiKey_Keypad9;
            case 111: return ImGuiKey_KeypadDivide;
            case 106: return ImGuiKey_KeypadMultiply;
            case 109: return ImGuiKey_KeypadSubtract;
            case 107: return ImGuiKey_KeypadAdd;
            case 17: return ImGuiKey_LeftCtrl;
            case 16: return ImGuiKey_LeftShift;
            case 18: return ImGuiKey_LeftAlt;
            case 91: return ImGuiKey_LeftSuper;
            case 162: return ImGuiKey_RightCtrl;
            case 160: return ImGuiKey_RightShift;
            case 164: return ImGuiKey_RightAlt;
            case 92: return ImGuiKey_RightSuper;
            case 93: return ImGuiKey_Menu;
            case 48: case 49: case 50: case 51: case 52:
            case 53: case 54: case 55: case 56: case 57:
                return static_cast<ImGuiKey>(ImGuiKey_0 + (keycode - 48)); // тгьиук 0 -9
            case 65: case 66: case 67: case 68: case 69:
            case 70: case 71: case 72: case 73: case 74:
            case 75: case 76: case 77: case 78: case 79:
            case 80: case 81: case 82: case 83: case 84:
            case 85: case 86: case 87: case 88: case 89: case 90:
                return static_cast<ImGuiKey>(ImGuiKey_A + (keycode - 65)); // a-z
            case 112: case 113: case 114: case 115: case 116:
            case 117: case 118: case 119: case 120: case 121:
            case 122: case 123:
                return static_cast<ImGuiKey>(ImGuiKey_F1 + (keycode - 112)); // f keys
            default: return ImGuiKey_None;
        }
    }



    ImGuiKey keycodeToImGuiMod(int keycode)
    {
        switch (keycode)
        {
            case 17:
            case 162:
                return ImGuiMod_Ctrl;
            case 16:
            case 160:
                return ImGuiMod_Shift;
            case 18:
            case 164:
                return ImGuiMod_Alt;
            case 91:
            case 92:
                return ImGuiMod_Super;
            default:
                break;
        }
        return ImGuiKey_None;
    }


    void onKey(KeyEvent &event)  {
        if (!SwapchainHook::init || !ImGui::GetCurrentContext()) return;
        if (!Client::disable) {
            ImGuiIO& io = ImGui::GetIO();
            const ImGuiKey mod = keycodeToImGuiMod(event.getKey());
            if (mod != ImGuiKey_None)
            {
                io.AddKeyEvent(mod, (int)event.getAction());
            }
            else
            {
                io.AddKeyEvent(ImGuiMod_Ctrl, false);
                io.AddKeyEvent(ImGuiMod_Shift, false);
                io.AddKeyEvent(ImGuiMod_Alt, false);
                io.AddKeyEvent(ImGuiMod_Super, false);
            }

            const ImGuiKey key = keycodeToImGuiKey(event.getKey());
            io.AddKeyEvent(key, (int)event.getAction());
            //io.SetKeyEventNativeData(key, static_cast<int>(event.getKey()), -1);


            int ch = ImGuiKeyToUnicode(mod);
            if(ch != -1) io.AddInputCharacter(ImGuiKeyToUnicode(mod));
        }
    };

    ImGUIKeyListener() {
        Listen(this, KeyEvent, &ImGUIKeyListener::onKey);
    }
    ~ImGUIKeyListener() {
        Deafen(this, KeyEvent, &ImGUIKeyListener::onKey);
    }
};
