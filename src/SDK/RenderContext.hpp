#pragma once

#include "SDK.hpp"

// Forward declarations (ScreenView and ScreenContext come from SDK.hpp includes)

class RenderContext {
public:
    static RenderContext& instance();

    // Screen view
    [[nodiscard]] ScreenView* getScreenView() const { return screenView_; }
    void setScreenView(ScreenView* view) { screenView_ = view; }

    // Screen context
    [[nodiscard]] ScreenContext* getScreenContext() const { return screenContext_; }
    void setScreenContext(ScreenContext* ctx) { screenContext_ = ctx; }

    // Draw text queues
    void pushDrawTextEntry(DrawTextQueueEntry entry);
    [[nodiscard]] std::vector<DrawTextQueueEntry>& getDrawTextQueue() { return drawTextQueue_; }
    [[nodiscard]] std::vector<DrawTextQueueEntry>& getDrawTextQueue2() { return drawTextQueue2_; }
    void clearDrawTextQueue() { drawTextQueue_.clear(); }
    void clearDrawTextQueue2() { drawTextQueue2_.clear(); }

    // Delete copy/move
    RenderContext(const RenderContext&) = delete;
    RenderContext& operator=(const RenderContext&) = delete;
    RenderContext(RenderContext&&) = delete;
    RenderContext& operator=(RenderContext&&) = delete;

private:
    RenderContext() = default;
    ~RenderContext() = default;

    ScreenView* screenView_ = nullptr;
    ScreenContext* screenContext_ = nullptr;
    std::vector<DrawTextQueueEntry> drawTextQueue_;
    std::vector<DrawTextQueueEntry> drawTextQueue2_;
};
