#include "../src/knxexception.h"
#include "../src/knxgroupobject/dptjsonconfig.h"
#include "config.h"
#include "testutils.h"
#include <gtest/gtest.h>

class DptJsonConfigTest: public ::testing::Test
{
protected:
    std::string SourceDir = CMAKE_SOURCE_DIR;

    void SetUp() override
    {
        DptJsonConfig = std::make_unique<knx::object::TDptJsonConfig>(SourceDir + "/wb-mqtt-knx-jsondpt.conf");
    }

    void TearDown() override
    {}

    std::unique_ptr<knx::object::TDptJsonConfig> DptJsonConfig;
};

TEST_F(DptJsonConfigTest, getConNameTest)
{
    auto name = DptJsonConfig->GetDptConfigName(knx::object::TDatapointId{5});
    ASSERT_TRUE(name);
    EXPECT_EQ("5.xxx_8-Bit_Unsigned_Value_JSON", *name);
}

TEST_F(DptJsonConfigTest, getConfigNameNegTest)
{
    auto name = DptJsonConfig->GetDptConfigName(knx::object::TDatapointId{5000});
    EXPECT_FALSE(name);
}
