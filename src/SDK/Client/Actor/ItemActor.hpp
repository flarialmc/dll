#pragma once

#include "Actor.hpp"

class ItemActor : public Actor {
public:
    ItemStack& getStack();
};
