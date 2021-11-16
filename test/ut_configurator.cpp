#include "../src/configurator.h"
#include "config.h"
#include "mock/groupobject_mock.h"
#include "mock/groupobjectmqttbuilder_mock.h"
#include "mock/knxgroupobjectcontroller_mock.h"
#include <gtest/gtest.h>
#include <inttypes.h>

using namespace testing;

class ConfiguratorTest: public ::testing::Test
{
protected:
    void SetUp() override
    {}

    void TearDown() override
    {}

    std::string SourceDir = CMAKE_SOURCE_DIR;
};

TEST_F(ConfiguratorTest, GetConfig)
{
    EXPECT_NO_THROW(std::make_unique<knx::TConfigurator>(SourceDir + "/test/config/wb-mqtt-knx.conf",
                                                         SourceDir + "/test/config/wb-mqtt-knx.schema.json",
                                                         nullptr));
}

TEST_F(ConfiguratorTest, GetIncorrectConfig)
{
    EXPECT_THROW(std::make_unique<knx::TConfigurator>(SourceDir + "/test/config/wb-mqtt-knx_bad.conf",
                                                      SourceDir + "/test/config/wb-mqtt-knx.schema.json",
                                                      nullptr),
                 std::exception);
}

TEST_F(ConfiguratorTest, NoConfig)
{
    EXPECT_THROW(std::make_unique<knx::TConfigurator>(".", SourceDir + "/test/config/wb-mqtt-knx.schema.json", nullptr),
                 std::exception);
}

TEST_F(ConfiguratorTest, CongigureController)
{
    auto goController = std::make_unique<StrictMock<TKnxGroupObjectControllerMock>>();

    EXPECT_CALL(*goController, AddGroupObject(_)).Times(4);

    auto groupObjectMqttBuilder = std::make_shared<TGroupObjectMqttBuilderMock>();

    EXPECT_CALL(*groupObjectMqttBuilder, LinkDevice("room1", _)).Times(1);
    EXPECT_CALL(*groupObjectMqttBuilder, LinkDevice("room2", _)).Times(1);

    uint32_t counter = 0;
    std::vector<knx::TKnxGroupAddress> addressList{{1, 1, 100}, {1, 2}, {1, 2, 56}, {1, 2, 140}};
    EXPECT_CALL(*groupObjectMqttBuilder, Create(_))
        .Times(4)
        .WillRepeatedly(Invoke([&counter, &addressList](const knx::object::TGroupObjectMqttParameter& parameter) {
            EXPECT_EQ(addressList[counter], parameter.KnxAddress);
            ++counter;
            return nullptr;
        }));

    auto param = knx::object::TGroupObjectMqttParameter{{1, 1, 100}, "", "", ""};

    auto configurator = std::make_unique<knx::TConfigurator>(SourceDir + "/test/config/wb-mqtt-knx.conf",
                                                             SourceDir + "/test/config/wb-mqtt-knx.schema.json",
                                                             groupObjectMqttBuilder);
    configurator->Configure(*goController);
}