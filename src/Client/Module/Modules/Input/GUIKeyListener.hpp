#include <format>
#include "../../../Events/Listener.hpp"
#include "../../../Events/Input/KeyEvent.hpp"
#include "../../../../Utils/Logger/Logger.hpp"
#include "../../../Client.hpp"
#include <Windows.h>

class GUIKeyListener : public Listener {

    bool isCapital = false;

    std::chrono::steady_clock::time_point lastOnKeyTime = std::chrono::steady_clock::now();

    void onKey(KeyEvent &event) override {

        if(!Client::disable) {

            if (ModuleManager::getModule("ClickGUI")->settings.getSettingByName<bool>("enabled")) {

                if (event.GetKey() == 20 && event.GetAction() == (int)ActionType::PRESSED || event.GetKey() == VK_SHIFT) isCapital = !isCapital;

                for(auto& shi : FlarialGUI::KeybindSelectors) {
                    shi.currentOnKeyTime = std::chrono::steady_clock::now();
                }

                if(event.GetPressedKeysAsString() != "no")
                FlarialGUI::currentKeybind = event.GetPressedKeysAsString();
                else FlarialGUI::currentKeybind = "nothing";

                for (auto &box: FlarialGUI::TextBoxes) {

                    if(event.GetKey() == VK_ESCAPE || ModuleManager::getModule("ClickGUI")->IsKeybind(event.keys) && ModuleManager::getModule("ClickGUI")->IsKeyPartOfKeybind(event.key)) box.isActive = false;

                    if (box.isActive && event.GetAction() == (int) ActionType::PRESSED)
                        if (event.GetKey() != VK_BACK)
                            box.text += event.GetKeyAsString(isCapital);
                        else {
                            if (!box.text.empty()) {
                                box.text.erase(box.text.length() - 1);  // Erase the last character
                            }
                        }
                }

            } else {

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
