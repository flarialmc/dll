
#pragma once

#include <vector>
#include "../Event.hpp"
#include "../Cancellable.hpp"

class ProtocolEvent : public Event {

    std::vector<std::pair<std::wstring, std::wstring>> args;
    std::wstring path;

public:
    std::vector<std::pair<std::wstring, std::wstring>> getProtocolArgs() {
        return this->args;
    }
    std::wstring getPath() { return this->path; }

    explicit ProtocolEvent(std::wstring path, std::vector<std::pair<std::wstring, std::wstring>> a) {
        this->args = a;
        this->path = path;
    }
};