#include <format>
#include "../../../Events/Listener.hpp"
#include "../../../Events/Input/KeyEvent.hpp"
#include "../../../../Utils/Logger/Logger.hpp"
#include "../../../Client.hpp"
#include <Windows.h>

// TODO: LIKELY REQ A FIX
class GUIKeyListener : public Listener {

    bool isCapital = false;

    std::chrono::steady_clock::time_point lastOnKeyTime = std::chrono::steady_clock::now();

    void onKey(KeyEvent &event) override {
        if (!Client::disable) {

            if (ModuleManager::getModule("ClickGUI")->active) {

                if (event.getKey() == 20 && event.getAction() == (int) ActionType::Pressed ||
                        event.getKey() == VK_SHIFT)
                    isCapital = !isCapital;

                for (auto &shi: FlarialGUI::KeybindSelectors) {
                    shi.currentOnKeyTime = std::chrono::steady_clock::now();
                }

                if (event.getPressedKeysAsString() != "no")
                    FlarialGUI::currentKeybind = event.getPressedKeysAsString();
                else FlarialGUI::currentKeybind = "nothing";

                for (auto &box: FlarialGUI::TextBoxes) {

                    if (event.getKey() == VK_ESCAPE || ModuleManager::getModule("ClickGUI")->isKeybind(event.keys) &&
                                                               ModuleManager::getModule("ClickGUI")->isKeyPartOfKeybind(
                                                                       event.key))
                        box.isActive = false;

                    if (event.getKey() == VK_BACK)
                        if (event.getAction() == (int) ActionType::Released) {
                            box.isDeleting = false;
                        }

                    if (box.isActive && event.getAction() == (int) ActionType::Pressed)

                        if (event.getKey() != VK_BACK && event.getPressedKeysAsString() != "CTRL+V")
                            box.text += event.getKeyAsString(isCapital);
                        else if (event.getKey() == VK_BACK) {

                            if (event.getAction() == (int) ActionType::Pressed) {

                                std::thread t([&box]() {

                                    bool firstTime = true;
                                    while (box.isDeleting) {

                                        if (!box.text.empty() && !firstTime) {
                                            box.text.erase(box.text.length() - 1);  // Erase the last character
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
                                box.text.erase(box.text.length() - 1);  // Erase the last character
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
                    box.isActive = false;
                }
            }
        }
    };

public:
    explicit GUIKeyListener(const char string[5]) {
        this->name = string;
    }
};
