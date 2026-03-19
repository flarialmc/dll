#include "Context.hpp"
#include <libhat/Access.hpp>

bgfx::RendererContextI* bgfx::Context::getRendererContext() {
    // 48 8B 8B ? ? ? ? 4C 8D 8B ? ? ? ? 48 8B 93
    return hat::member_at<RendererContextI*>(this, 0xC6D7250);
}
