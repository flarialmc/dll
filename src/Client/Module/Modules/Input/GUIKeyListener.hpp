#include <format>
#include "../../../Events/Listener.hpp"
#include "../../../Events/Input/KeyEvent.hpp"
#include "../../../../Utils/Logger/Logger.hpp"
#include <Windows.h>

class GUIKeyListener : public Listener {

    bool isCapital = false;

    void onKey(KeyEvent &event) override {

        if(ModuleManager::getModule("ClickGUI")->settings.getSettingByName<bool>("enabled")) {

            if (event.GetKey() == 20) isCapital = !isCapital;

            for (auto &box: FlarialGUI::TextBoxes) {

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

    };

public:
    explicit GUIKeyListener(const char string[5]) {
        this->name = string;
    }
};
