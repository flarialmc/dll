#pragma once

#include <Command/Command.hpp>

class ExportGeoCommand : public Command {
public:
    ExportGeoCommand() : Command("exportgeo", "Export a player's skin geometry to a JSON file.",
                                 {"geo", "geometry"}) {}

    void execute(const std::vector<std::string>& args) override;
};
