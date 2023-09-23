#include "../Hook.hpp"
#include "../../../Events/EventHandler.hpp"
#include "../../../../SDK/SDK.hpp"

class getViewPerspectiveHook : public Hook {

private:
    static int callback(uintptr_t* a1);

public:
    typedef int(__thiscall* original)(uintptr_t*);

    static inline original getViewPerspectiveOriginal = nullptr;

    getViewPerspectiveHook() : Hook("getViewPerspective", "48 83 EC ? 48 8B 01 48 8D 54 ? ? 41 B8 03 00 00 00") {}

    void enableHook() override;
};