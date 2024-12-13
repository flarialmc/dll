#include "PathCommand.hpp"

#include <Utils/WinrtUtils.hpp>

void PathCommand::execute(const std::vector<std::string> &args) {
    WinrtUtils::openSubFolder("Flarial");
}