#include "ViewModel.hpp"
#include "Client.hpp"
#include "Events/EventManager.hpp"
#include "glm/glm/ext/matrix_transform.hpp"

ViewModel::ViewModel(): Module("View Model", "Allows you to modify how item in hand looks.", IDR_EYE_PNG, "C")
{
    
}

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
    if (Matrixed) {
        auto& matrix = SDK::clientInstance->getCamera().getWorldMatrixStack().top().matrix;
        matrix = OriginalMatrix;
    }

    Module::onDisable();
}

void ViewModel::defaultConfig()
{
    Module::defaultConfig("core");
    setDef("itemfov", 70.0f);
    setDef("thirdperson", false);
    setDef("posx", 4.0f);
    setDef("posy", 4.0f);
    setDef("posz", 4.0f);
    setDef("rotangle", 0.0f);
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


    addHeader("View Model");
    addToggle("Third Person", "Transforms the item in third person perspective", "thirdperson");
    addSlider("Item FOV", "Changes the FOV appearance of the item.", "itemfov", 180);

    addSlider("Position X", "Changes the position in the X axis", "posx", 12);
    addSlider("Position Y", "Changes the position in the Y axis", "posy", 12);
    addSlider("Position Z", "Changes the position in the Z axis", "posz", 12);

    addSlider("Rotation Angle", "Changes the rotation angle of the item", "rotangle", 360);
    addSlider("Rotation X", "Changes the rotation in the X axis", "rotx", 360);
    addSlider("Rotation Y", "Changes the rotation in the Y axis", "roty", 360);
    addSlider("Rotation Z", "Changes the rotation in the Z axis", "rotz", 360);

    addSlider("Scale X", "Changes the scale in the X axis", "scalex", 3, -3);
    addSlider("Scale Y", "Changes the scale in the Y axis", "scaley", 3, -3);
    addSlider("Scale Z", "Changes the scale in the Z axis", "scalez", 3, -3);

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
    if (thirdperson && getOps<bool>("thirdperson") || !thirdperson) {
        auto& matrix = SDK::clientInstance->getCamera().getWorldMatrixStack().top().matrix;
        if (!Matrixed) OriginalMatrix = matrix;

        auto posx = getOps<float>("posx");
        auto posy = getOps<float>("posy");
        auto posz = getOps<float>("posz");

        auto rotx = getOps<float>("rotx");
        auto roty = getOps<float>("roty");
        auto rotz = getOps<float>("rotz");

        auto scalex = getOps<float>("scalex");
        auto scaley = getOps<float>("scaley");
        auto scalez = getOps<float>("scalez");


        auto rotAngle = getOps<float>("rotangle");

        matrix = glm::translate<float>(matrix, glm::vec3(posx - 4, posy - 4, posz - 4));
        matrix = glm::rotate<float>(matrix, glm::radians(rotAngle), glm::vec3(rotx, roty, rotz));
        matrix = glm::scale(matrix, glm::vec3(scalex, scaley, scalez));
    }
}
