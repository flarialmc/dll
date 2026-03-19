//
// Created by Leslie on 12/31/2025.
//

#pragma once

#include <thread>

#include "Client.hpp"

using ScheduledFunc = std::function<void()>;
class ThreadingUtils
{
public:
    // instance getter
    static ThreadingUtils& getInstance()
    {
        static ThreadingUtils instance;
        return instance;
    }

    std::vector<std::function<void()>> mMainThreadQueue;
    mutable std::mutex mMainThreadMutex;

    static bool isMainThread() {
        return std::this_thread::get_id() == Client::g_mainThreadId;
    }

    void queueInMainThread(ScheduledFunc&& func)
    {
        std::lock_guard lock(mMainThreadMutex);
        mMainThreadQueue.push_back(std::forward<ScheduledFunc>(func));
    };

    void executeMainThreadQueue()
    {
        // This function would contain logic to execute tasks queued for the main thread.
        // Implementation depends on how tasks are queued and stored.
        mMainThreadMutex.lock();
        const auto queue = std::move(mMainThreadQueue);
        mMainThreadQueue = {};
        mMainThreadMutex.unlock();

        // Call the queue
        for (auto& task : queue)
        {
            task();
        }
    }
};
