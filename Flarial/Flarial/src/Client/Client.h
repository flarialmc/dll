#pragma once
#include "Managers/Hooks/Manager.h"
#include "Managers/Frontend/Manager.h"
#include "Managers/Modules/Manager.h"

class Client
{
public:
    static HooksManager* HooksManager;
    static FrontendManager* FrontendManager;
    static ModulesManager* ModulesManager;

public:
    static void initialize();
};
