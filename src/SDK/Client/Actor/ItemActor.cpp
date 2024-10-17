#include "ItemActor.hpp"

ItemStack& ItemActor::getStack() {
    static auto off = GET_OFFSET("ItemActor::stack");
    return hat::member_at<ItemStack>(this, off);
}
