#pragma once

namespace knx
{
    /// Service errors
    enum class TKnxEvent
    {
        None = 0,
        ReceivedTelegram,
        KnxdSocketConnected,
        KnxdSocketError,
        PollReadTimeoutError
    };
}
