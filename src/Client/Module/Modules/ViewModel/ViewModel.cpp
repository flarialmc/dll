#include "ViewModel.hpp"
#include "Client.hpp"

#include "glm/glm/ext/matrix_transform.hpp"


void ViewModel::onEnable()
{
    Listen(this, FOVEvent, &ViewModel::onGetFOV)
    Listen(this, RenderItemInHandEvent, &ViewModel::onRenderItemInHand)
    Listen(this, PerspectiveEvent, &ViewModel::onGetPerspective)
    Module::onEnable();
}

void ViewModel::onDisable()
{
    Deafen(this, FOVEvent, &ViewModel::onGetFOV)
    Deafen(this, RenderItemInHandEvent, &ViewModel::onRenderItemInHand)
    Deafen(this, PerspectiveEvent, &ViewModel::onGetPerspective)
    Module::onDisable();
}

void ViewModel::defaultConfig()
{
    Module::defaultConfig("core");
    setDef("itemfov", 70.0f);
    setDef("thirdperson", false);
    setDef("posx", 0.0f);
    setDef("posy", 0.0f);
    setDef("posz", 0.0f);
    // Separate rotation angles around each axis (in degrees)
    setDef("rotx", 0.0f);
    setDef("roty", 0.0f);
    setDef("rotz", 0.0f);
    setDef("scalex", 1.0f);
    setDef("scaley", 1.0f);
    setDef("scalez", 1.0f);
}

void ViewModel::settingsRender(float settingsOffset)
{
    initSettingsPage();


    addToggle("Third Person", "Transforms the item in third person perspective", "thirdperson");
    addSlider("Item FOV", "Changes the FOV appearance of the item.", "itemfov", 180);

    addSlider("Position X", "Moves the item left/right", "posx", 4, -4, false);
    addSlider("Position Y", "Moves the item up/down", "posy", 4, -4, false);
    addSlider("Position Z", "Moves the item forward/backward", "posz", 4, -4, false);

    addSlider("Rotation X", "Rotates around the X axis (pitch)", "rotx", 180, -180, false);
    addSlider("Rotation Y", "Rotates around the Y axis (yaw)", "roty", 180, -180, false);
    addSlider("Rotation Z", "Rotates around the Z axis (roll)", "rotz", 180, -180, false);

    addSlider("Scale X", "Changes the scale in the X axis", "scalex", 3, -3, false);
    addSlider("Scale Y", "Changes the scale in the Y axis", "scaley", 3, -3, false);
    addSlider("Scale Z", "Changes the scale in the Z axis", "scalez", 3, -3, false);

    FlarialGUI::UnsetScrollView();
    resetPadding();
}

void ViewModel::onGetPerspective(PerspectiveEvent& event)
{
    if (!this->isEnabled()) return;
    if (event.getPerspective() == Perspective::FirstPerson) thirdperson = false;
    else thirdperson = true;
}

void ViewModel::onGetFOV(FOVEvent& event)
{
    if (!this->isEnabled()) return;
    auto fov = event.getFOV();
    if (fov != 70) return;

    event.setFOV(getOps<float>("itemfov"));
}

void ViewModel::onRenderItemInHand(RenderItemInHandEvent& event)
{
    if (!this->isEnabled()) return;
    if ((thirdperson && getOps<bool>("thirdperson")) || !thirdperson)
    {
        auto& matrix = SDK::clientInstance->getCamera().getWorldMatrixStack().top().matrix;

        auto posx = getOps<float>("posx");
        auto posy = getOps<float>("posy");
        auto posz = getOps<float>("posz");

        // Rotation angles around each axis (in degrees)
        auto rotx = getOps<float>("rotx");
        auto roty = getOps<float>("roty");
        auto rotz = getOps<float>("rotz");

        auto scalex = getOps<float>("scalex");
        auto scaley = getOps<float>("scaley");
        auto scalez = getOps<float>("scalez");

        // Apply translation
        if (posx != 0.0f || posy != 0.0f || posz != 0.0f)
        {
            matrix = glm::translate<float>(matrix, glm::vec3(posx, posy, posz));
        }

        // Apply separate rotations around each axis (order: X, Y, Z)
        if (rotx != 0.0f)
        {
            matrix = glm::rotate<float>(matrix, glm::radians(rotx), glm::vec3(1.0f, 0.0f, 0.0f));
        }
        if (roty != 0.0f)
        {
            matrix = glm::rotate<float>(matrix, glm::radians(roty), glm::vec3(0.0f, 1.0f, 0.0f));
        }
        if (rotz != 0.0f)
        {
            matrix = glm::rotate<float>(matrix, glm::radians(rotz), glm::vec3(0.0f, 0.0f, 1.0f));
        }

        // Apply scale
        if (scalex != 1.0f || scaley != 1.0f || scalez != 1.0f)
        {
            matrix = glm::scale(matrix, glm::vec3(scalex, scaley, scalez));
        }
    }
}