#pragma once

#include "../Event.hpp"
#include "../Cancellable.hpp"

enum MouseAction {
    Press = 1,
    Release = 2,
    ScrollUp = 3,
    ScrollDown = 4
};

enum MouseButton {
    None = 0, Left = 1, Right = 2, Middle = 3, Scroll = 4, Button5 = 5, Button6 = 6
};

class MouseEvent : public Event, public Cancellable {

private:
    MouseButton button;
    MouseAction action;
    char actionRaw;
    short mouseX;
    short mouseY;
public:

    MouseEvent(char button, char action, short mouseX, short mouseY) : Event() {
        this->button = (MouseButton) button;
        this->action = (MouseAction) action;
        this->setActionFromChar(action);
        this->actionRaw = action;
        this->mouseX = mouseX;
        this->mouseY = mouseY;
    };

    [[nodiscard]] auto getButton() const -> MouseButton {
        return this->button;
    }

    void setButton(MouseButton newButton) {
        this->button = newButton;
    }

    [[nodiscard]] auto getAction() const -> MouseAction {
        return this->action;
    }

    [[nodiscard]] auto getActionRaw() const -> int {
        return this->actionRaw;
    }

    [[nodiscard]] auto getActionAsChar() const -> char {
        switch (this->action) {
            case MouseAction::Release:
                return (char)0;
            case MouseAction::Press:
                return (char)1;
            case MouseAction::ScrollUp:
                return (char)0x78;
            case MouseAction::ScrollDown:
                return (char)0x80;
            default:
                return (char) this->action;
        }
    }

    void setAction(MouseAction newAction) {
        this->action = newAction;
    }

    void setActionFromChar(char newAction) {
        switch (newAction) {
            case 0:
                this->action = MouseAction::Release;
                break;
            case 1:
                this->action = MouseAction::Press;
                break;
            case 0x7F:
            case 0x78:
                this->action = MouseAction::ScrollUp;
                break;
            case 0x80:
            case 0x88:
                this->action = MouseAction::ScrollDown;
                break;
            default:
                this->action = (MouseAction) newAction;
                break;
        }
    }

    [[nodiscard]] auto getMouseX() const {
        return this->mouseX;
    }

    void setMouseX(short newMouseY) {
        this->mouseX = newMouseY;
    }

    [[nodiscard]] auto getMouseY() const {
        return this->mouseY;
    }

    void setMouseY(short newMouseY) {
        this->mouseY = newMouseY;
    }
};
