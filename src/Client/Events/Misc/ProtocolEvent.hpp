
#pragma once

#include <vector>
#include <Events/Event.hpp>

class ProtocolEvent : public Event {
public:
    explicit ProtocolEvent(const std::wstring& path, const std::vector<std::pair<std::wstring, std::wstring>>& a) {
        this->args = a;
        this->path = path;
    }

    std::vector<std::pair<std::wstring, std::wstring>> getProtocolArgs() {
        return this->args;
    }

    std::wstring getPath() { return this->path; }
private:
    std::vector<std::pair<std::wstring, std::wstring>> args;
    std::wstring path;
};