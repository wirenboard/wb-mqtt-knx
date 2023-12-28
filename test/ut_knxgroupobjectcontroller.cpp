#include "../src/knxgroupobjectcontroller.h"

#include "../src/observer.h"
#include "../src/ticktimer.h"
#include "mock/groupobject_mock.h"
#include "mock/knxtelegramsender.h"
#include <gtest/gtest.h>
#include <utility>

using namespace ::testing;

class TEventObserverStub: public knx::TObserver<knx::TKnxEvent, knx::TTelegram>
{
public:
    void NotifyAll(const knx::TKnxEvent& event, const knx::TTelegram& telegram)
    {
        NotifyAllSubscribers(event, telegram);
    }
};

class TTickTimerObserverStub: public knx::TObserver<knx::TTickTimerEvent>
{
public:
    void NotifyAll(const knx::TTickTimerEvent& event)
    {
        NotifyAllSubscribers(event);
    }
};

class KnxGroupObjectControllerTest: public ::testing::Test
{
protected:
    void SetUp() override
    {
        TelegramSender = std::make_shared<TTelegramSenderMock>();
        Controller = std::make_shared<knx::TKnxGroupObjectController>(TelegramSender, TickTime);
    }

    void TearDown() override
    {}

    std::shared_ptr<knx::TKnxGroupObjectController> Controller;
    std::shared_ptr<TTelegramSenderMock> TelegramSender;
    std::chrono::milliseconds TickTime{10};
};

TEST_F(KnxGroupObjectControllerTest, AddObject)
{
    knx::TKnxGroupAddress address{"1/1/1"};
    knx::TGroupObjectSettings goSettings;
    goSettings.GroupAddress = address;
    auto groupObjectMock = std::make_shared<TGroupObjectMock>();
    EXPECT_CALL(*groupObjectMock, SetKnxSender(address, _)).Times(1);

    EXPECT_TRUE(Controller->AddGroupObject(groupObjectMock, goSettings));
    EXPECT_FALSE(Controller->AddGroupObject(groupObjectMock, goSettings));
}

TEST_F(KnxGroupObjectControllerTest, RemoveObject)
{
    knx::TKnxGroupAddress address{"1/1/1"};
    knx::TGroupObjectSettings goSettings;
    goSettings.GroupAddress = address;
    auto groupObjectMock = std::make_shared<TGroupObjectMock>();
    EXPECT_CALL(*groupObjectMock, SetKnxSender(address, _)).Times(1);

    EXPECT_TRUE(Controller->AddGroupObject(groupObjectMock, goSettings));
    EXPECT_TRUE(Controller->RemoveGroupObject(address));
    EXPECT_FALSE(Controller->RemoveGroupObject(address));
}

TEST_F(KnxGroupObjectControllerTest, ReadRequestAfterStart)
{
    TEventObserverStub eventObserverStub;
    TTickTimerObserverStub tickTimerObserverStub;

    knx::TKnxGroupAddress address{"1/1/1"};
    knx::TGroupObjectSettings goSettings;
    goSettings.GroupAddress = address;
    goSettings.ReadRequestAfterStart = true;
    auto groupObjectMock = std::make_shared<TGroupObjectMock>();
    EXPECT_CALL(*groupObjectMock, SetKnxSender(address, _)).Times(1);
    EXPECT_CALL(*TelegramSender, Send(_)).Times(1);

    EXPECT_TRUE(Controller->AddGroupObject(groupObjectMock, goSettings));
    eventObserverStub.Subscribe(Controller);
    tickTimerObserverStub.Subscribe(Controller);

    eventObserverStub.NotifyAll(knx::TKnxEvent::KnxdSocketConnected, knx::TTelegram{});
}

TEST_F(KnxGroupObjectControllerTest, SendTelegram)
{
    TEventObserverStub eventObserverStub;
    TTickTimerObserverStub tickTimerObserverStub;
    knx::object::PSenderGroupObject goSender;

    knx::TKnxGroupAddress address{"1/1/1"};
    std::vector<uint8_t> payload = {10, 20};

    knx::TGroupObjectSettings goSettings;
    goSettings.GroupAddress = address;
    goSettings.ReadRequestAfterStart = false;
    auto groupObjectMock = std::make_shared<TGroupObjectMock>();
    EXPECT_CALL(*groupObjectMock, SetKnxSender(address, _))
        .WillOnce(Invoke([&goSender](const knx::TKnxGroupAddress&, knx::object::PSenderGroupObject sender) {
            goSender = std::move(sender);
        }));
    EXPECT_CALL(*TelegramSender, Send(_)).WillOnce(Invoke([&payload, &address](const knx::TTelegram& telegram) {
        EXPECT_EQ(telegram.Tpdu().GetPayload(), payload);
        EXPECT_EQ(telegram.GetReceiverAddress(), address.GetEibAddress());
    }));

    EXPECT_TRUE(Controller->AddGroupObject(groupObjectMock, goSettings));
    goSender->Send(knx::object::TGroupObjectTransaction{address, knx::telegram::TApci::GroupValueWrite, payload});
    eventObserverStub.Subscribe(Controller);
    tickTimerObserverStub.Subscribe(Controller);

    eventObserverStub.NotifyAll(knx::TKnxEvent::KnxdSocketConnected, knx::TTelegram{});
}

TEST_F(KnxGroupObjectControllerTest, RecvTelegram)
{
    TEventObserverStub eventObserverStub;
    TTickTimerObserverStub tickTimerObserverStub;

    knx::TKnxGroupAddress address{"1/1/1"};
    std::vector<uint8_t> payload = {10, 20};

    knx::TTelegram knxTelegram{};
    knxTelegram.SetReceiverAddress(address.GetEibAddress());
    knxTelegram.SetGroupAddressedFlag(true);
    knxTelegram.Tpdu().SetAPCI(knx::telegram::TApci::GroupValueWrite);
    knxTelegram.Tpdu().SetPayload(payload);

    knx::TGroupObjectSettings goSettings;
    goSettings.GroupAddress = address;
    goSettings.ReadRequestAfterStart = false;
    auto groupObjectMock = std::make_shared<TGroupObjectMock>();
    EXPECT_CALL(*groupObjectMock, SetKnxSender(address, _)).Times(1);
    EXPECT_CALL(*groupObjectMock, KnxNotify(_))
        .WillOnce(Invoke([&knxTelegram](const knx::object::TGroupObjectTransaction& transaction) {
            EXPECT_EQ(knxTelegram.GetReceiverAddress(), transaction.Address.GetEibAddress());
            EXPECT_EQ(knxTelegram.Tpdu().GetAPCI(), transaction.Apci);
            EXPECT_EQ(knxTelegram.Tpdu().GetPayload(), transaction.Payload);
        }));

    EXPECT_TRUE(Controller->AddGroupObject(groupObjectMock, goSettings));

    eventObserverStub.Subscribe(Controller);
    tickTimerObserverStub.Subscribe(Controller);

    eventObserverStub.NotifyAll(knx::TKnxEvent::KnxdSocketConnected, knx::TTelegram{});
    eventObserverStub.NotifyAll(knx::TKnxEvent::ReceivedTelegram, knxTelegram);
}

TEST_F(KnxGroupObjectControllerTest, RecvFeedbackTelegram)
{
    TEventObserverStub eventObserverStub;
    TTickTimerObserverStub tickTimerObserverStub;

    knx::TKnxGroupAddress address{"1/1/1"};
    knx::TKnxGroupAddress feedbackAddress{"1/2/1"};
    std::vector<uint8_t> payload = {10, 20};

    knx::TTelegram knxFeedbackTelegram{};
    knxFeedbackTelegram.SetReceiverAddress(feedbackAddress.GetEibAddress());
    knxFeedbackTelegram.SetGroupAddressedFlag(true);
    knxFeedbackTelegram.Tpdu().SetAPCI(knx::telegram::TApci::GroupValueWrite);
    knxFeedbackTelegram.Tpdu().SetPayload(payload);

    knx::TGroupObjectSettings goSettings;
    goSettings.GroupAddress = address;
    goSettings.ReadRequestAfterStart = false;
    goSettings.FeedbackGroupAddress = feedbackAddress;

    auto groupObjectMock = std::make_shared<TGroupObjectMock>();
    EXPECT_CALL(*groupObjectMock, SetKnxSender(address, _)).Times(1);
    EXPECT_CALL(*groupObjectMock, KnxNotify(_))
        .WillOnce(Invoke([&knxFeedbackTelegram](const knx::object::TGroupObjectTransaction& transaction) {
            EXPECT_EQ(knxFeedbackTelegram.GetReceiverAddress(), transaction.Address.GetEibAddress());
            EXPECT_EQ(knxFeedbackTelegram.Tpdu().GetAPCI(), transaction.Apci);
            EXPECT_EQ(knxFeedbackTelegram.Tpdu().GetPayload(), transaction.Payload);
        }));

    EXPECT_TRUE(Controller->AddGroupObject(groupObjectMock, goSettings));

    eventObserverStub.Subscribe(Controller);
    tickTimerObserverStub.Subscribe(Controller);

    eventObserverStub.NotifyAll(knx::TKnxEvent::KnxdSocketConnected, knx::TTelegram{});
    eventObserverStub.NotifyAll(knx::TKnxEvent::ReceivedTelegram, knxFeedbackTelegram);
}

TEST_F(KnxGroupObjectControllerTest, PollRead)
{
    TEventObserverStub eventObserverStub;
    TTickTimerObserverStub tickTimerObserverStub;

    knx::TKnxGroupAddress address{"1/1/1"};
    std::vector<uint8_t> payload = {10, 20};

    knx::TTelegram knxTelegram{};
    knxTelegram.SetReceiverAddress(address.GetEibAddress());
    knxTelegram.SetGroupAddressedFlag(true);
    knxTelegram.Tpdu().SetAPCI(knx::telegram::TApci::GroupValueResponse);
    knxTelegram.Tpdu().SetPayload(payload);

    knx::TGroupObjectSettings goSettings;
    goSettings.GroupAddress = address;
    goSettings.ReadRequestAfterStart = false;
    goSettings.ReadRequestPollInterval = std::chrono::milliseconds(10 * TickTime);

    auto groupObjectMock = std::make_shared<TGroupObjectMock>();
    EXPECT_CALL(*groupObjectMock, SetKnxSender(address, _)).Times(1);
    EXPECT_CALL(*groupObjectMock, KnxNotify(_))
        .WillOnce(Invoke([&knxTelegram](const knx::object::TGroupObjectTransaction& transaction) {
            EXPECT_EQ(knxTelegram.GetReceiverAddress(), transaction.Address.GetEibAddress());
            EXPECT_EQ(knxTelegram.Tpdu().GetAPCI(), transaction.Apci);
            EXPECT_EQ(knxTelegram.Tpdu().GetPayload(), transaction.Payload);
        }));

    EXPECT_TRUE(Controller->AddGroupObject(groupObjectMock, goSettings));
    EXPECT_CALL(*TelegramSender, Send(_)).WillOnce(Invoke([](const knx::TTelegram& telegram) {
        EXPECT_EQ(telegram.Tpdu().GetAPCI(), knx::telegram::TApci::GroupValueRead);
    }));

    eventObserverStub.Subscribe(Controller);
    tickTimerObserverStub.Subscribe(Controller);

    eventObserverStub.NotifyAll(knx::TKnxEvent::KnxdSocketConnected, knx::TTelegram{});
    for (int i = 0; i < (goSettings.ReadRequestPollInterval / TickTime + 1); ++i) {
        tickTimerObserverStub.NotifyAll(knx::TTickTimerEvent::TimeIsUp);
    }
    eventObserverStub.NotifyAll(knx::TKnxEvent::ReceivedTelegram, knxTelegram);
}

TEST_F(KnxGroupObjectControllerTest, PollReadTimeout)
{
    TEventObserverStub eventObserverStub;
    TTickTimerObserverStub tickTimerObserverStub;

    knx::TKnxGroupAddress address{"1/1/1"};
    std::vector<uint8_t> payload = {10, 20};

    knx::TTelegram knxTelegram{};
    knxTelegram.SetReceiverAddress(address.GetEibAddress());
    knxTelegram.SetGroupAddressedFlag(true);
    knxTelegram.Tpdu().SetAPCI(knx::telegram::TApci::GroupValueResponse);
    knxTelegram.Tpdu().SetPayload(payload);

    knx::TGroupObjectSettings goSettings;
    goSettings.GroupAddress = address;
    goSettings.ReadRequestAfterStart = false;
    goSettings.ReadRequestPollInterval = std::chrono::milliseconds(10 * TickTime);
    goSettings.ReadResponseTimeout = std::chrono::milliseconds(5 * TickTime);

    auto groupObjectMock = std::make_shared<TGroupObjectMock>();
    EXPECT_CALL(*groupObjectMock, SetKnxSender(address, _)).Times(1);
    EXPECT_CALL(*groupObjectMock, KnxNotify(_))
        .WillOnce(Invoke([&knxTelegram](const knx::object::TGroupObjectTransaction& transaction) {
            EXPECT_EQ(knxTelegram.GetReceiverAddress(), transaction.Address.GetEibAddress());
            EXPECT_EQ(knxTelegram.Tpdu().GetAPCI(), transaction.Apci);
            EXPECT_EQ(knxTelegram.Tpdu().GetPayload(), transaction.Payload);
        }));

    EXPECT_CALL(*groupObjectMock, KnxNotifyEvent(_)).WillOnce(Invoke([](const knx::TKnxEvent& event) {
        EXPECT_EQ(event, knx::TKnxEvent::PollReadTimeoutError);
    }));

    EXPECT_TRUE(Controller->AddGroupObject(groupObjectMock, goSettings));
    EXPECT_CALL(*TelegramSender, Send(_)).WillOnce(Invoke([](const knx::TTelegram& telegram) {
        EXPECT_EQ(telegram.Tpdu().GetAPCI(), knx::telegram::TApci::GroupValueRead);
    }));

    eventObserverStub.Subscribe(Controller);
    tickTimerObserverStub.Subscribe(Controller);

    eventObserverStub.NotifyAll(knx::TKnxEvent::KnxdSocketConnected, knx::TTelegram{});
    for (int i = 0; i < (2 * goSettings.ReadRequestPollInterval / TickTime - 1); ++i) {
        tickTimerObserverStub.NotifyAll(knx::TTickTimerEvent::TimeIsUp);
    }
    eventObserverStub.NotifyAll(knx::TKnxEvent::ReceivedTelegram, knxTelegram);
}