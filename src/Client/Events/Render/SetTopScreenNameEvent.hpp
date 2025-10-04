#pragma once

#include "../Event.hpp"
#include "../Cancellable.hpp"
#include "../../../SDK/SDK.hpp"

class SetTopScreenNameEvent : public Event {
private:
    std::string layer;

public:
    explicit SetTopScreenNameEvent(const std::string& layer) {
        this->layer = layer;
    }

    void setCustomLayer(const std::string& layer) {
        this->layer = layer;
    }

    std::string getLayer() {
        return this->layer;
    }
};