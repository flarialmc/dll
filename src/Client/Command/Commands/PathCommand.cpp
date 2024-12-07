#include "PathCommand.hpp"
#include "imgui.h"

void PathCommand::execute(const std::vector<std::string> &args) {
    //TODO: Use winrt to open folder automatically
    ImGui::SetClipboardText(Utils::getClientPath().c_str());
    addCommandMessage("§aSuccessfully copied! Use Win + R and paste to open.");
}
