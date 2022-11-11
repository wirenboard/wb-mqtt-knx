#include "ticktimer.h"
#include "knxexception.h"
#include "wblib/log.h"
#include "wblib/utils.h"

using namespace knx;

void TTickTimer::Start()
{
    std::lock_guard<std::mutex> lg(ActiveMutex);

    bool expectedIsStartedValue = false;

    if (!IsStarted.compare_exchange_strong(expectedIsStartedValue, true)) {
        wb_throw(knx::TKnxException, "Attempt to start already started driver");
    }

    Worker = WBMQTT::MakeThread("wb-mqtt-knx TimerThread", {[this] { Processing(); }});
}

void TTickTimer::Stop()
{
    std::lock_guard<std::mutex> lg(ActiveMutex);

    bool expectedIsStartedValue = true;
    if (!IsStarted.compare_exchange_strong(expectedIsStartedValue, false)) {
        ::WBMQTT::Error.Log() << "Attempt to stop already stopped TTickTimer";
        return;
    }

    if (Worker != nullptr && Worker->joinable()) {
        Worker->join();
    }
}
void TTickTimer::Processing()
{
    while (IsStarted) {
        std::this_thread::sleep_for(TickPeriod.load());
        NotifyAllSubscribers(TTickTimerEvent::TimeIsUp);
    }
}
void TTickTimer::SetTickPeriod(std::chrono::milliseconds period)
{
    TickPeriod = period;
}
