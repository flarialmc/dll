#pragma once
#include "Managers/Hook/Manager.hpp"
#include "Managers/Module/Manager.hpp"

class Client
{
public:
    HooksManager* HooksManager;
    ModulesManager* ModulesManager;
    
    void initialize();

    class HooksManager getHooksManager() const;
    class ModulesManager getModulesManager() const;
};
