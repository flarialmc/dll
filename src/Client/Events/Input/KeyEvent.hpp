
#pragma once
#include "../Event.hpp"
#include "../Cancellable.hpp"

enum class ActionType {
    RELEASED = 0,
    PRESSED = 1
};

class KeyEvent : public Event, public Cancellable {

public:
    int key;
    int action;

    KeyEvent(int key, int action) : Event() {
        this->key = key;
        this->action = action;
    };


    [[nodiscard]] int GetKey() const {
        return key;
    }

    void SetKey(int e) {
        this->key = e;
    }

    [[nodiscard]] int GetAction() {
        return this->action;
    }

    int SetAction(int e) {
        this->action = e;
    }
};



