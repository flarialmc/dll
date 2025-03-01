#pragma once

#include "ScriptLib.hpp"
#include <ImGui/imgui.h>

class ImGuiLib : public ScriptLib {
public:
    void initialize(lua_State* state) override {
        using namespace luabridge;

        getGlobalNamespace(state)
            .beginNamespace("ImGui")
                .addFunction("Begin", [](const char* name) {
                    ImGui::Begin(name);
                })
                .addFunction("End", []() {
                    ImGui::End();
                })
                .addFunction("BeginChild", [](const char* name, const LuaRef& sizeTable, ImGuiChildFlags child_flags = 0, ImGuiWindowFlags window_flags = 0) {
                    ImVec2 size = toImVec2(sizeTable);
                    ImGui::BeginChild(name, size, child_flags, window_flags);
                })
                .addFunction("BeginChildID", [](ImGuiID id, const LuaRef& sizeTable, ImGuiChildFlags child_flags = 0, ImGuiWindowFlags window_flags = 0) {
                    ImVec2 size = toImVec2(sizeTable);
                    ImGui::BeginChild(id, size, child_flags, window_flags);
                })
                .addFunction("BeginChildFrame", [](ImGuiID id, const LuaRef& sizeTable, ImGuiWindowFlags flags = 0) {
                    ImVec2 size = toImVec2(sizeTable);
                    ImGui::BeginChildFrame(id, size, flags);
                })
                .addFunction("BeginCombo", [](const char* name, const char* previewValue, ImGuiComboFlags flags = 0) {
                    ImGui::BeginCombo(name, previewValue, flags);
                })
                .addFunction("BeginDragDropSource", [](ImGuiDragDropFlags flags = 0) {
                    ImGui::BeginDragDropSource(flags);
                })
                .addFunction("BeginDragDropTarget", []() {
                    ImGui::BeginDragDropTarget();
                })
                .addFunction("BeginGroup", []() {
                    ImGui::BeginGroup();
                })
                .addFunction("BeginMainMenuBar", []() {
                    ImGui::BeginMainMenuBar();
                })
                .addFunction("BeginMenu", [](const char* name, bool enabled = true) {
                    ImGui::BeginMenu(name, enabled);
                })
                .addFunction("BeginMenuBar", []() {
                    ImGui::BeginMenuBar();
                })
                .addFunction("BeginPopup", [](const char* str_id, ImGuiWindowFlags flags = 0) {
                    ImGui::BeginPopup(str_id, flags);
                })
                .addFunction("BeginPopupContextItem", [](const char* str_id = 0, ImGuiPopupFlags popup_flags = 1) {
                    ImGui::BeginPopupContextItem(str_id, popup_flags);
                })
                .addFunction("BeginPopupContextVoid", [](const char* str_id = 0, ImGuiPopupFlags popup_flags = 1) {
                    ImGui::BeginPopupContextVoid(str_id, popup_flags);
                })
                .addFunction("BeginTabBar", [](const char* str_id, ImGuiTabBarFlags flags = 0) {
                    ImGui::BeginTabBar(str_id, flags);
                })
                .addFunction("BeginTable", [](const char* name, int column, const LuaRef& outer_sizeTable, float inner_width = 0) {
                    ImVec2 outer_size = toImVec2(outer_sizeTable);
                    ImGui::BeginTable(name, column, 0, outer_size, inner_width);
                })
                .addFunction("BeginTooltip", []() {
                    ImGui::BeginTooltip();
                })
                .addFunction("Bullet", []() {
                    ImGui::Bullet();
                })
                .addFunction("BulletText", [](const char* text) {
                    ImGui::BulletText(text);
                })
                .addFunction("Button", [](const char* label, const LuaRef& sizeTable) {
                    ImVec2 size = toImVec2(sizeTable);
                    return ImGui::Button(label, size);
                })
                .addFunction("CloseCurrentPopup", []() {
                    ImGui::CloseCurrentPopup();
                })
                .addFunction("CollapsingHeader", [](const char* label, ImGuiTreeNodeFlags flags = 0) {
                    return ImGui::CollapsingHeader(label, flags);
                })
                .addFunction("ColorButton", [](const char* desc_id, const LuaRef& colTable, const LuaRef& sizeTable) {
                    ImVec4 col = toImVec4(colTable);
                    ImVec2 size = toImVec2(sizeTable);
                    return ImGui::ColorButton(desc_id, col, 0, size);
                })
                .addFunction("Text", [](const char* text) {
                    ImGui::Text(text);
                })
                .addFunction("GetBackgroundDrawList", []() {
                    return ImGui::GetBackgroundDrawList();
                })
                .addFunction("GetForegroundDrawList", []() {
                    return ImGui::GetForegroundDrawList();
                })
                .addFunction("GetWindowDrawList", []() {
                    return ImGui::GetWindowDrawList();
                })
                .addFunction("SameLine", [](float offset_from_start_x = 0, float spacing = -1) {
                    return ImGui::SameLine(offset_from_start_x, spacing);
                })
                .addFunction("GetIO", []() {
                    return &ImGui::GetIO();
                })
                .addFunction("GetDrawData", []() {
                    return ImGui::GetDrawData();
                })
                .addFunction("SetNextWindowSize", [](const LuaRef& sizeTable, ImGuiCond cond = 0) {
                    ImVec2 size = toImVec2(sizeTable);
                    ImGui::SetNextWindowSize(size, cond);
                })
                .addFunction("SetNextWindowPos", [](const LuaRef& posTable, const LuaRef& pivotTable) {
                    ImVec2 pos = toImVec2(posTable);
                    ImVec2 pivot = toImVec2(pivotTable);
                    ImGui::SetNextWindowPos(pos, 0, pivot);
                })
                .addFunction("SetNextWindowBgAlpha", [](float alpha) {
                    ImGui::SetNextWindowBgAlpha(alpha);
                })
                .addFunction("SetNextWindowCollapsed", [](bool collapsed) {
                    ImGui::SetNextWindowCollapsed(collapsed);
                })
                .addFunction("SetNextWindowFocus", []() {
                    ImGui::SetNextWindowFocus();
                })
                .addFunction("SetNextWindowContentSize", [](const LuaRef& sizeTable) {
                    ImVec2 size = toImVec2(sizeTable);
                    ImGui::SetNextWindowContentSize(size);
                })
                .addFunction("IsKeyDown", [](int key) {
                    return ImGui::IsKeyDown(static_cast<ImGuiKey>(key));
                })
                .addFunction("IsKeyPressed", [](int key, bool repeat = true) {
                    return ImGui::IsKeyPressed(static_cast<ImGuiKey>(key));
                })
                .addFunction("IsKeyReleased", [](int key) {
                    return ImGui::IsKeyReleased(static_cast<ImGuiKey>(key));
                })
                .addFunction("IsMouseDown", [](int button) {
                    return ImGui::IsMouseDown(button);
                })
                .addFunction("IsMouseClicked", [](int button, bool repeat = false) {
                    return ImGui::IsMouseClicked(button, repeat);
                })
                .addFunction("IsMouseReleased", [](int button) {
                    return ImGui::IsMouseReleased(button);
                })
            .endNamespace()
            .beginClass<ImDrawList>("ImDrawList")
                .addFunction("AddLine", [](ImDrawList* drawList, const LuaRef& p1Table, const LuaRef& p2Table, const LuaRef& colorTable, float thickness = 1) {
                    ImVec2 p1 = toImVec2(p1Table);
                    ImVec2 p2 = toImVec2(p2Table);
                    ImColor color = toColor(colorTable);
                    drawList->AddLine(p1, p2, color, thickness);
                })
                .addFunction("AddRect", [](ImDrawList* drawList, const LuaRef& p1Table, const LuaRef& p2Table, const LuaRef& colorTable, float rounding = 0, ImDrawFlags flags = 0, float thickness = 1) {
                    ImVec2 p1 = toImVec2(p1Table);
                    ImVec2 p2 = toImVec2(p2Table);
                    ImColor color = toColor(colorTable);
                    drawList->AddRect(p1, p2, color, rounding, flags, thickness);
                })
                .addFunction("AddRectFilled", [](ImDrawList* drawList, const LuaRef& p1Table, const LuaRef& p2Table, const LuaRef& colorTable, float rounding = 0, ImDrawFlags flags = 0) {
                    ImVec2 p1 = toImVec2(p1Table);
                    ImVec2 p2 = toImVec2(p2Table);
                    ImColor color = toColor(colorTable);
                    drawList->AddRectFilled(p1, p2, color, rounding, flags);
                })
                .addFunction("AddCircle", [](ImDrawList* drawList, const LuaRef& centerTable, float radius, const LuaRef& colorTable, int numSegments = 0, float thickness = 1) {
                    ImVec2 center = toImVec2(centerTable);
                    ImColor color = toColor(colorTable);
                    drawList->AddCircle(center, radius, color, numSegments, thickness);
                })
                .addFunction("AddCircleFilled", [](ImDrawList* drawList, const LuaRef& centerTable, float radius, const LuaRef& colorTable, int numSegments = 0) {
                    ImVec2 center = toImVec2(centerTable);
                    ImColor color = toColor(colorTable);
                    drawList->AddCircleFilled(center, radius, color, numSegments);
                })
                .addFunction("AddText", [](ImDrawList* drawList, const LuaRef& posTable, const LuaRef& colorTable, const char* text_begin, const char* text_end = 0) {
                    ImVec2 pos = toImVec2(posTable);
                    ImColor color = toColor(colorTable);
                    drawList->AddText(pos, color, text_begin, text_end);
                })
                .addFunction("AddTriangleFilled", [](ImDrawList* drawList, const LuaRef& p1Table, const LuaRef& p2Table, const LuaRef& p3Table, const LuaRef& colorTable) {
                    ImVec2 p1 = toImVec2(p1Table);
                    ImVec2 p2 = toImVec2(p2Table);
                    ImVec2 p3 = toImVec2(p3Table);
                    ImColor color = toColor(colorTable);
                    drawList->AddTriangleFilled(p1, p2, p3, color);
                })
                .addFunction("AddTriangle", [](ImDrawList* drawList, const LuaRef& p1Table, const LuaRef& p2Table, const LuaRef& p3Table, const LuaRef& colorTable, float thickness = 1) {
                    ImVec2 p1 = toImVec2(p1Table);
                    ImVec2 p2 = toImVec2(p2Table);
                    ImVec2 p3 = toImVec2(p3Table);
                    ImColor color = toColor(colorTable);
                    drawList->AddTriangle(p1, p2, p3, color, thickness);
                })
                .addFunction("AddQuadFilled", [](ImDrawList* drawList, const LuaRef& p1Table, const LuaRef& p2Table, const LuaRef& p3Table, const LuaRef& p4Table, const LuaRef& colorTable) {
                    ImVec2 p1 = toImVec2(p1Table);
                    ImVec2 p2 = toImVec2(p2Table);
                    ImVec2 p3 = toImVec2(p3Table);
                    ImVec2 p4 = toImVec2(p4Table);
                    ImColor color = toColor(colorTable);
                    drawList->AddQuadFilled(p1, p2, p3, p4, color);
                })
                .addFunction("AddQuad", [](ImDrawList* drawList, const LuaRef& p1Table, const LuaRef& p2Table, const LuaRef& p3Table, const LuaRef& p4Table, const LuaRef& colorTable, float thickness = 1) {
                    ImVec2 p1 = toImVec2(p1Table);
                    ImVec2 p2 = toImVec2(p2Table);
                    ImVec2 p3 = toImVec2(p3Table);
                    ImVec2 p4 = toImVec2(p4Table);
                    ImColor color = toColor(colorTable);
                    drawList->AddQuad(p1, p2, p3, p4, color, thickness);
                })
            .endClass()
            .beginClass<ImGuiIO>("ImGuiIO")
                .addProperty("DisplaySize", &ImGuiIO::DisplaySize)
                .addProperty("DeltaTime", &ImGuiIO::DeltaTime)
                .addProperty("Framerate", &ImGuiIO::Framerate)
            .endClass()
            .beginClass<std::vector<ImVec2>>("ImVec2Vector")
                .addFunction("size", &std::vector<ImVec2>::size)
                .addFunction("get", [](std::vector<ImVec2>* vec, size_t index) -> ImVec2 {
                    if (index < 1 || index > vec->size()) {
                        return {};
                    }
                    return (*vec)[index - 1];
                })
                .addFunction("__len", &std::vector<ImVec2>::size)
                .addFunction("__index", [](std::vector<ImVec2>* vec, size_t index) -> ImVec2 {
                    if (index < 1 || index > vec->size()) {
                        return {};
                    }
                    return (*vec)[index - 1];
                })
            .endClass();
    }
};