#pragma once

namespace knx
{
    /// Service errors
    enum class TKnxError
    {
        None = 0,
        KnxdSocketError,
        PoolReadTimeoutError
    };
}
