#pragma once

#include "observer.h"
#include <atomic>
#include <chrono>
#include <mutex>
#include <thread>

namespace knx
{
    /// Tick timer event
    enum class TTickTimerEvent
    {
        TimeIsUp = 0 ///< The time interval has ended
    };

    /// Service for generating periodic events
    class TTickTimer: public TObserver<TTickTimerEvent>
    {
    public:
        /// Set timer period
        /// \param period time duration
        void SetTickPeriod(std::chrono::milliseconds period);

        /// Start service
        void Start();

        /// Stop service
        void Stop();

    private:
        std::mutex ActiveMutex;
        void Processing();
        std::atomic<bool> IsStarted{false};
        std::unique_ptr<std::thread> Worker;
        std::atomic<std::chrono::milliseconds> TickPeriod;
    };
}
