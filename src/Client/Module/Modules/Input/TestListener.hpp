#include <format>
#include "../../../Events/Listener.hpp"
#include "../../../Events/Input/KeyEvent.hpp"
#include "../../../../Utils/Logger/Logger.hpp"
#include <Windows.h>

class TestListener : public Listener {

    void onKey(KeyEvent &event) override {

        for (auto& box : FlarialGUI::TextBoxes) {

            if(box.isActive && event.GetAction() == (int)ActionType::PRESSED)
                box.text += event.GetKeyAsString();
        }

    };

public:
    explicit TestListener(const char string[5]) {
        this->name = string;
    }
};
