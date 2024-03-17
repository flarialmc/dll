#pragma once

#include "../Event.hpp"
#include "../Cancellable.hpp"
#include <format>

class DrawTextEvent : public Event, Cancellable {
private:
    std::string* text;
public:
    [[nodiscard]] std::string* getText() {
        return this->text;
    }

    void setText(std::string* e) {
        this->text = e;
    }

    explicit DrawTextEvent(std::string* text) : text(text) {}
};