#pragma once
#include "Managers/Hooks/Manager.h"
#include "Managers/Frontend/Manager.h"
#include "Managers/Modules/Manager.h"

class Client
{
public:
    HooksManager* HooksManager;
    FrontendManager* FrontendManager;
    ModulesManager* ModulesManager;

public:
    void initialize();
};
