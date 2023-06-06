#pragma once
#include <vector>

#include "../BaseManager.hpp"
#include "BaseHook.hpp"

class HooksManager: public BaseManager
{
public:
    std::vector<BaseHook*> hooks;
};
