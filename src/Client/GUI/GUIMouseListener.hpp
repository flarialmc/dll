#include <format>
#include <Windows.h>
#include "../Events/Listener.hpp"
#include "../Events/Input/MouseEvent.hpp"
#include "../../Utils/Logger/Logger.hpp"
#include "D2D.hpp"

class GUIMouseListener : public Listener {

    void onMouse(MouseEvent &event) override {

       GUIInfo::mousepos.x = event.mouseX;
       GUIInfo::mousepos.y = event.mouseY;

    };

public:
    explicit GUIMouseListener(const char string[5]) {
        this->name = string;
    }
};
