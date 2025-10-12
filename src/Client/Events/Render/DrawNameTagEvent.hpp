#pragma once

#include "../Event.hpp"
#include "SDK/Client/Render/NameTagRenderObject.hpp"

class DrawNameTagEvent : public Event {
private:
    NameTagRenderObject* tagRenderObject;
public:
    [[nodiscard]] NameTagRenderObject* getTagRenderObject() {
        return this->tagRenderObject;
    }

    explicit DrawNameTagEvent(NameTagRenderObject* _tagRenderObject) : tagRenderObject(_tagRenderObject) {}
};