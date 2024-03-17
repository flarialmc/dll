#pragma once

class Cancellable {
private:
    bool _cancelled;
public:
    Cancellable() {
        this->_cancelled = false;
    }

    [[nodiscard]] bool isCancelled() const {
        return this->_cancelled;
    }

    void setCancelled(bool cancelled) {
        this->_cancelled = cancelled;
    }

    void cancel() {
        this->_cancelled = true;
    }
};
