#pragma once

#include <unordered_map>
#include <string>
#include <functional>
#include <sstream>
#include "../../SDK/SDK.hpp"

namespace CommandManager {
    inline std::unordered_map<std::string, std::function<void(const std::vector<std::string>&)>> commands;
    inline const std::string prefix = ".";

    inline void command(const std::string& name, const std::function<void(const std::vector<std::string>&)>& func) {
        commands[name] = func;
    }

    inline bool executeCommand(const std::string& input) {
        if (input.empty() || input[0] != prefix[0]) {
            return false;
        }

        std::istringstream stream(input);
        std::string commandName;
        stream >> commandName;

        commandName.erase(0, 1);

        if (commands.find(commandName) == commands.end()) {
            SDK::clientInstance->getGuiData()->displayClientMessage("§cCommand was not found!§r Use .help for command list.");
            return true;
        }

        std::vector<std::string> args;
        std::string arg;
        while (stream >> arg) {
            args.push_back(arg);
        }

        commands[commandName](args);
        return true;
    }
};