#pragma once

#include <utility>

#include "../../SDK/SDK.hpp"

class Command {
public:
    std::string Name;
    std::string Description;
    std::vector<std::string> Aliases;

    //TODO: add it to config in the future?
    inline static std::string prefix = ".";

    Command(std::string  name, std::string  description,
            const std::vector<std::string>& aliases = {})
        : Name(std::move(name)), Description(std::move(description)), Aliases(aliases) {}

    virtual ~Command() = default;
    virtual void execute(const std::vector<std::string>& args) = 0;

    void addCommandMessage(const std::string& fmt, auto&& ... args) const {
        try {
            std::string message = fmt::format(fmt::runtime(fmt), std::forward<decltype(args)>(args)...);
            std::string formattedMessage = fmt::format("§r§l§9{} §3§l»§r {}", Name, message);

            SDK::clientInstance->getGuiData()->displayClientMessage(formattedMessage);
        } catch (const std::exception& e) {
            Logger::error("addCommandMessage error: {}", e.what());
        }
    }
};