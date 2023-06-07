#include <format>
#include "../../../Events/Listener.hpp"
#include "../../../Events/Input/KeyEvent.hpp"
#include "../../../../Utils/Logger/Logger.hpp"
#include <Windows.h>

class TestListener : public Listener {

    void onKey(KeyEvent &event) override {

    if(event.GetKey() == 'A') Logger::info("This shit also work");
    };

public:
    explicit TestListener(const char string[5]) {
        this->name = string;
    }
};
