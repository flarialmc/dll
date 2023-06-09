#pragma once
#include "../Event.hpp"
#include "../Cancellable.hpp"

enum class MouseAction {
    PRESS,
    RELEASE,
    SCROLL_UP,
    SCROLL_DOWN
};

enum MouseButton { None = 0, Left = 1, Right = 2, Middle = 3, Scroll = 4, Button5 = 5, Button6 = 6};

class MouseEvent : public Event, public Cancellable {

public:
    MouseButton button;
    MouseAction action;
    int actionRaw;
    short mouseX;
    short mouseY;
public:
    MouseEvent(char button, char action, short mouseX, short mouseY) : Event() {
        this->button = (MouseButton)button;
        this->action = (MouseAction)action;
        this->SetActionFromChar(action);
        this->actionRaw = action;
        this->mouseX = mouseX;
        this->mouseY = mouseY;
    };

    auto GetButton() -> MouseButton {
        return this->button;
    }
    void SetButton(MouseButton button) {
        this->button = button;
    }
    auto GetAction() -> MouseAction {
        return this->action;
    }

    auto GetActionRaw() -> int {
        return this->actionRaw;
    }

    auto GetActionAsChar() -> char {
        switch(this->action) {
            case MouseAction::RELEASE: return 0;
            case MouseAction::PRESS: return 1;
            case MouseAction::SCROLL_UP: return 0x78;
            case MouseAction::SCROLL_DOWN: return 0x80;
            default: return (char)this->action;
        }
    }
    void SetAction(MouseAction action) {
        this->action = action;
    }
    void SetActionFromChar(char action) {
        switch(action) {
            case 0: this->action = MouseAction::RELEASE; break;
            case 1: this->action = MouseAction::PRESS; break;
            case 0x78: this->action = MouseAction::SCROLL_UP; break;
            case 0x7F: this->action = MouseAction::SCROLL_UP; break;
            case 0x80: this->action = MouseAction::SCROLL_DOWN; break;
            case 0x88: this->action = MouseAction::SCROLL_DOWN; break;
            default: this->action = (MouseAction)action; break;
        }
    }

    short GetMouseX() const {
        return this->mouseX;
    }
    void SetMouseX(short mouseX) {
        this->mouseX = mouseX;
    }
    short GetMouseY() const {
        return this->mouseY;
    }
    void SetMouseY(short mouseY) {
        this->mouseY = mouseY;
    }


};
