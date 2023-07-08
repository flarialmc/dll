#include "../Hook.hpp"
#include "../../../Events/EventHandler.hpp"
#include "../../../../SDK/SDK.hpp"
class OnSuspendHook : public Hook {

private:
    static void callback(void* a1, void* a2, void* a3, void* a4);

public:
    typedef void(__thiscall* original)(void*, void*, void*, void*);

    static inline original suspendOriginal = nullptr;

   OnSuspendHook() : Hook("OnSuspendHook", "48 89 5C 24 ? 48 89 74 24 ? 55 57 41 56 48 8D AC 24 ? ? ? ? 48 81 EC ? ? ? ? 48 8B 05 ? ? ? ? 48 33 C4 48 89 85 ? ? ? ? 4C 8B F1 48 8D B1 ? ? ? ?") {}

    void enableHook() override;
};