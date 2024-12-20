﻿#pragma once

#include "../../SDK/SDK.hpp"
#include "../Client.hpp"

class Command {
public:
    std::string Name;
    std::string Description;
    std::vector<std::string> Aliases;


    Command(const std::string& name, const std::string& description,
        const std::vector<std::string>& aliases = {})
        : Name(name), Description(description), Aliases(aliases) {}

    virtual ~Command() = default;
    virtual void execute(const std::vector<std::string>& args) = 0;

    void addCommandMessage(const std::string& fmt, auto&& ... args) const {
        try {
            std::string message = fmt::format(fmt::runtime(fmt), std::forward<decltype(args)>(args)...);
            std::string formattedMessage = fmt::format("§r§l§9{} §3§l»§r {}", Name, message);

            SDK::clientInstance->getGuiData()->displayClientMessage(formattedMessage);
        }
        catch (const std::exception& e) {
            Logger::error("addCommandMessage error: {}", e.what());
        }
    }
};