
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
    ActionType action;

    KeyEvent(int key, int action) : Event() {
        this->key = key;
        this->action = (ActionType)action;
    };

    int GetKey() const{
        return this->key;
    }

    void SetKey(int e) {
        this->key = e;
    }

    ActionType GetAction() const {
        return this->action;
    }

    int SetAction(ActionType e) {
        this->action = e;
    }
};



