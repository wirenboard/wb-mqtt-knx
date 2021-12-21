#include "ticktimer.h"
#include "knxexception.h"
#include "wblib/utils.h"

using namespace knx;

void TTickTimer::Start()
{
    bool expectedIsStartedValue = false;

    if (!IsStarted.compare_exchange_strong(expectedIsStartedValue, true)) {
        wb_throw(knx::TKnxException, "Attempt to start already started driver");
    }

    Worker = WBMQTT::MakeThread("wb-mqtt-knx TimerThread", {[this] { Processing(); }});
}

void TTickTimer::Stop()
{
    bool expectedIsStartedValue = true;
    if (!IsStarted.compare_exchange_strong(expectedIsStartedValue, false)) {
        wb_throw(knx::TKnxException, "Attempt to stop already stopped TTickTimer");
    }

    if (Worker->joinable()) {
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
