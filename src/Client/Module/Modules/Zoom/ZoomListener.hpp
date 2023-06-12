#include <format>
#include "../../../Events/Listener.hpp"
#include "../../../Events/Input/KeyEvent.hpp"
#include "../../../../Utils/Logger/Logger.hpp"
#include "../Module.hpp"
#include <Windows.h>

class ZoomListener : public Listener {

    Module* module;

    void onKey(KeyEvent &event) override {

        if(event.GetKey() == module->keybind) module->enabled = !module->enabled;
    };

public:
    explicit ZoomListener(const char string[5], Module* module) {
        this->name = string;
        this->module = module;
    }
};
