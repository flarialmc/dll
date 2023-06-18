#include <format>
#include "../../../Events/Listener.hpp"
#include "../../../Events/Input/KeyEvent.hpp"
#include "../../../../Utils/Logger/Logger.hpp"
#include <Windows.h>

class GUIKeyListener : public Listener {

    bool isCapital = false;

    void onKey(KeyEvent &event) override {

        if(event.GetKey() == 20) isCapital = !isCapital;

        for (auto& box : FlarialGUI::TextBoxes) {

            if(box.isActive && event.GetAction() == (int)ActionType::PRESSED)
                box.text += event.GetKeyAsString(isCapital);
        }

    };

public:
    explicit GUIKeyListener(const char string[5]) {
        this->name = string;
    }
};
