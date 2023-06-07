#pragma once
class Cancellable {
    bool cancelled;
public:
    Cancellable() {
        this->cancelled = false;
    }

    bool isCancelled() {
        return this->cancelled;
    }
    void setCancelled(bool cancelled) {
        this->cancelled = cancelled;
    }
    void cancel() {
        this->cancelled = true;
    }
};
