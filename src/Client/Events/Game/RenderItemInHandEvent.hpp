#pragma once

#include "../Event.hpp"
#include <glm/glm/glm.hpp>

class RenderItemInHandEvent : public Event {
public:
    ItemStack* itemStack;

    RenderItemInHandEvent(ItemStack* itemStack)
        : itemStack(itemStack) {}
};
