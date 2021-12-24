#pragma once

#pragma once

#include "../../src/isender.h"
#include "../../src/knxtelegram.h"
#include "gmock/gmock.h"

class TTelegramSenderMock: public knx::ISender<knx::TTelegram>
{
public:
    MOCK_METHOD1(Send, void(const knx::TTelegram& telegram));
};
