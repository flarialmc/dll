#pragma once
#include <vector>

#include "../BaseManager.h"

class HooksManager: public BaseManager
{
public:
    std::vector<Hook*> hooks;
};
