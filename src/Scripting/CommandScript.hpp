#pragma once

#include "Script.hpp"
#include <Client/Command/Command.hpp>

class CommandScript : public Command {
public:
    std::weak_ptr<Script> linkedScript;

    CommandScript(const std::string& name, const std::string& description, const std::vector<std::string>& aliases, std::shared_ptr<Script> script)
        : Command(name, description, aliases), linkedScript(script) {}

    void execute(const std::vector<std::string>& args) override {
        if (auto script = linkedScript.lock()) {
            script->registerCommandEvent("execute", args);
        }
    }
};