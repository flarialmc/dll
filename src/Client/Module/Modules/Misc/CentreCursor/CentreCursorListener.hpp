#pragma once
#include "../../../../Events/Listener.hpp"
#include "../../../../Client.hpp"
class CentreCursorListener : public Listener {

public:
    explicit CentreCursorListener(const char string[5]) {
        this->name = string;
    }
};
