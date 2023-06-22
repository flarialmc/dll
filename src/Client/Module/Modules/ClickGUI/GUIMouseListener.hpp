#pragma once
#include <format>
#include <Windows.h>
#include "../../../Events/Listener.hpp"
#include "../../../Events/Input/MouseEvent.hpp"
#include "../../../../Utils/Logger/Logger.hpp"
#include "../../../GUI/D2D.hpp"
#include "../../../GUI/Engine/Engine.hpp"
#include "../../../Module/Manager.hpp"

class GUIMouseListener : public Listener {

    void onMouse(MouseEvent &event) override {

       MC::mousepos.x = event.mouseX;
       MC::mousepos.y = event.mouseY;
       MC::mousebutton = event.GetButton();
       MC::mouseaction = event.GetAction();

       if(event.GetButton() != MouseButton::None && event.GetAction() == MouseAction::PRESS) MC::held = true;
        if(event.GetButton() != MouseButton::None && event.GetAction() == MouseAction::RELEASE) MC::held = false;

        if(event.GetButton() == MouseButton::Scroll)
        {
            accumilatedPos += (event.GetAction() == MouseAction::SCROLL_UP) ? FlarialGUI::scrollposmodifier : -FlarialGUI::scrollposmodifier;
            accumilatedBarPos += (event.GetAction() == MouseAction::SCROLL_UP) ? FlarialGUI::barscrollposmodifier : -FlarialGUI::barscrollposmodifier;
        }

        if(ModuleManager::getModule("ClickGUI")->settings.getSettingByName<bool>("enabled")->value) event.setCancelled(true);

    };

public:

    static float inline accumilatedPos = 1;
    static float inline accumilatedBarPos = 1;
    explicit GUIMouseListener(const char string[5]) {
        this->name = string;
    }
};
