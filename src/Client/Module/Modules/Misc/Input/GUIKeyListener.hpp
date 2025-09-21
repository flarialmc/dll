#include <format>
#include "../../../../Events/Input/KeyEvent.hpp"
#include "../../../../../Utils/Logger/Logger.hpp"
#include "../../../../Client.hpp"
#include <windows.h>
#include <unknwn.h>
#include <cctype>

// TODO: LIKELY REQ A FIX
class GUIKeyListener : public Listener {
private:
    bool isCapital = false;
public:
    void onKey(KeyEvent &event)  {
        if (!Client::disable) {

            if (ModuleManager::getModule("ClickGUI")->active) {

                if (event.getKey() == 20 && event.getAction() == ActionType::Pressed ||
                        event.getKey() == VK_SHIFT)
                    isCapital = !isCapital;

                for (auto &shi: FlarialGUI::KeybindSelectors) {
                    shi.second.currentOnKeyTime = std::chrono::steady_clock::now();
                }

                if (event.getPressedKeysAsString() != "no")
                    FlarialGUI::currentKeybind = event.getPressedKeysAsString();
                else FlarialGUI::currentKeybind = "nothing";

                for (auto &troll: FlarialGUI::TextBoxes) {

                    auto& box = troll.second;
                    if (event.getKey() == VK_ESCAPE || ModuleManager::getModule("ClickGUI")->isKeybind(event.keys) &&
                                                               ModuleManager::getModule("ClickGUI")->isKeyPartOfKeybind(
                                                                       event.key))
                        box.isActive = false;

                    if (event.getKey() == VK_BACK)
                        if (event.getAction() == ActionType::Released) {
                            box.isDeleting = false;
                        }

                    if (box.isActive && event.getAction() == ActionType::Pressed)

                        if (event.getKey() != VK_BACK && event.getPressedKeysAsString() != "CTRL+V") {
                            int key = event.getKey();
                            bool isAllowedChar = false;

                            if ((key >= 48 && key <= 57) || // 0-9
                                (key >= 65 && key <= 90))   // A-Z (handles both cases as getKeyAsString converts)
                                {
                                    isAllowedChar = true;
                                }
                            else {
                                switch (key) {
                                    case 188: // , or <
                                    case 190: // . or >
                                    case 192: // ` or ~
                                    case 219: // [ or {
                                    case 221: // ] or }
                                    case 220: // \ or |
                                    case 222: // ' or "
                                    case 191: // / or ?
                                    case 187: // = or +
                                    case 189: // - or _
                                    case 186: // ; or :
                                        isAllowedChar = true;
                                        break;
                                    case 32: // Spacebar
                                        isAllowedChar = true;
                                        break;
                                        // Add other specific allowed symbols here
                                    default:
                                        break;
                                }
                            }

                            if (isAllowedChar) box.text += event.getKeyAsString(isCapital);





                            // box.text += event.getKeyAsString(isCapital);
                        } else if (event.getKey() == VK_BACK) {

                            if (event.getAction() == ActionType::Pressed) {

                                std::thread t([&box]() {

                                    bool firstTime = true;
                                    while (box.isDeleting) {

                                        if (!box.text.empty() && !firstTime) {
                                            if (GetAsyncKeyState(VK_CONTROL) & 0x8000) {
                                                size_t pos = box.text.length();

                                                while (pos > 0 && std::isspace(box.text[pos - 1])) {
                                                    pos--;
                                                }

                                                while (pos > 0 && !std::isspace(box.text[pos - 1])) {
                                                    pos--;
                                                }

                                                box.text.erase(pos);
                                            } else {
                                                box.text.erase(box.text.length() - 1);
                                            }
                                        }

                                        if (firstTime) {
                                            std::this_thread::sleep_for(std::chrono::milliseconds(400));
                                            firstTime = false;
                                        }
                                        else {
                                            std::this_thread::sleep_for(std::chrono::milliseconds(100));
                                        }
                                    }
                                });

                                t.detach();

                                box.isDeleting = true;

                            }

                            if (!box.text.empty()) {
                                if (GetAsyncKeyState(VK_CONTROL) & 0x8000) {
                                    size_t pos = box.text.length();

                                    while (pos > 0 && std::isspace(box.text[pos - 1])) {
                                        pos--;
                                    }

                                    while (pos > 0 && !std::isspace(box.text[pos - 1])) {
                                        pos--;
                                    }

                                    box.text.erase(pos);
                                } else {
                                    box.text.erase(box.text.length() - 1);
                                }
                            }
                        }

                    if (box.isActive && event.getPressedKeysAsString() == "CTRL+V") {

                        if (OpenClipboard(nullptr)) {
                            HANDLE hData = GetClipboardData(CF_TEXT);
                            if (hData != nullptr) {
                                char *clipboardText = static_cast<char *>(GlobalLock(hData));
                                if (clipboardText != nullptr) {
                                    box.text += clipboardText;
                                    GlobalUnlock(hData);
                                }
                            }
                            CloseClipboard();
                        }
                    }
                }

            } else {
                // fix perfomace ? Worked even without this :/
                for (auto &box: FlarialGUI::TextBoxes) {
                    box.second.isActive = false;
                }
            }
        }
    };

    GUIKeyListener() {
        Listen(this, KeyEvent, &GUIKeyListener::onKey);
    }
    ~GUIKeyListener() {
        Deafen(this, KeyEvent, &GUIKeyListener::onKey);
    }
};
