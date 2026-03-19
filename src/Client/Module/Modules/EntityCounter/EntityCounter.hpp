#pragma once

#include "../HUDModule.hpp"
#include "Events/Render/SetupAndRenderEvent.hpp"

class EntityCounter : public HUDModule {
public:
    EntityCounter() : HUDModule(24, "Entity Counter", "Counts the entities in the surrounding area",
        IDR_ENTITYCOUNTER_PNG, "") {}

protected:
    std::string getDisplayValue() override;

    void customInit() override;

    void customCleanup() override;

private:
    int entityCount = 0;

    void onSetupAndRender(SetupAndRenderEvent& event);
};
