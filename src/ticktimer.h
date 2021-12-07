#pragma once

#include "observer.h"
#include <atomic>
#include <chrono>
#include <thread>

namespace knx
{
    enum class TTickTimerEvent
    {
        TimeIsUp = 0
    };

    class TTickTimer: public TObserver<TTickTimerEvent>
    {
    public:
        void SetTickPeriod(std::chrono::milliseconds period);
        void Start();
        void Stop();

    private:
        void Processing();
        std::atomic<bool> IsStarted{false};
        std::unique_ptr<std::thread> Worker;
        std::atomic<std::chrono::milliseconds> TickPeriod;
    };
}
