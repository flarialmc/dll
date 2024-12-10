#pragma once

#include <vector>
#include "imgui.h"
#include "fmt/format.h"
#include "fmt/color.h"

class ScriptingConsole {
private:
    struct ConsoleLog {
        std::string text;
        fmt::text_style style;
    };

    std::vector <ConsoleLog> logs;
    bool scrollToBottom = false;
public:
    void addLog(const std::string &name, const std::string &message, const fmt::text_style &style) {
        logs.emplace_back(ConsoleLog{"[" + name + "] " + message, style});
    }

    void render() {
        ImGui::SetNextWindowSize(ImVec2(800, 600));
        if (ImGui::Begin("Console", nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize)) {

            ImGui::BeginChild("ScrollingRegion", ImVec2(0, 0), true, ImGuiWindowFlags_HorizontalScrollbar);
            for (const auto &log: logs) {
                auto styledMessage = fmt::format(log.style, "{}", log.text);
                ImGui::TextUnformatted(styledMessage.c_str());
            }

            ImGui::EndChild();
        }
        ImGui::End();
    }
};
