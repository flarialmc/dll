#pragma once
#include <vector>

#include "../BaseManager.h"
#include "BaseHook.h"

class HooksManager: public BaseManager
{
public:
    std::vector<BaseHook*> hooks;
};
