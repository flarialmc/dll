#include "RenderContext.hpp"

RenderContext& RenderContext::instance() {
    static RenderContext instance;
    return instance;
}

void RenderContext::pushDrawTextEntry(DrawTextQueueEntry entry) {
    drawTextQueue_.push_back(std::move(entry));
}
