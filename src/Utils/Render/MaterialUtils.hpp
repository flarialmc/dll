#pragma once

#include "../../SDK/Client/Render/Material/MaterialPtr.hpp"

class MaterialUtils {
public:
    static void update();

    static mce::MaterialPtr* getUIFillColor();
    static mce::MaterialPtr* getUITextured();
    static mce::MaterialPtr* getParticlesAlpha();
    static mce::MaterialPtr* getUICrosshair();
    static mce::MaterialPtr* getNametag();
};
