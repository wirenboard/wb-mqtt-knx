#include "../src/knxexception.h"
#include "../src/knxgroupobject/dptjson.h"
#include "gtest/gtest.h"

class DptJsonTest: public ::testing::Test
{
protected:
    void SetUp() override
    {
        Dpt = std::make_unique<knx::object::TDptJson>();
    }

    void TearDown() override
    {}

    std::unique_ptr<knx::object::TDptJson> Dpt;
    std::vector<std::pair<int32_t, std::vector<uint8_t>>> Samples = {{0, {0x00, 0x00, 0x00}},
                                                                     {32767, {0x00, 0x7F, 0xFF}},
                                                                     {12345, {0x00, 0x30, 0x39}},
                                                                     {-1, {0x00, 0xFF, 0xFF}},
                                                                     {-32768, {0x00, 0x80, 0x00}},
                                                                     {254, {0x00, 0x00, 254}}};
};

// TEST_F(DptJsonTest, InitVal)
//{
//     EXPECT_EQ(Samples.at(0).first, Dpt->ToMqtt().at(0).As<std::string>());
//     EXPECT_EQ(Samples.at(0).second, Dpt->ToKnx());
// }

TEST_F(DptJsonTest, toMqttTest)
{
    knx::object::TDptJsonField jsonUintField("uintField1", knx::object::TDptJsonField::EFieldType::UNSIGNED_INT, 16);
    knx::object::TDptJsonField jsonUintField2("uintField2", knx::object::TDptJsonField::EFieldType::UNSIGNED_INT, 8);
    knx::object::TDptJsonField jsonBitField("bitField", knx::object::TDptJsonField::EFieldType::BIT, 1);

    Dpt->AddField(jsonUintField, 8);
    Dpt->AddField(jsonUintField2, 24);
    Dpt->AddField(jsonBitField, 39);
    Dpt->FromKnx({0x00, 0xAA, 0xBB, 0xCC, 0x01});
    std::cout << Dpt->ToMqtt()[0].As<std::string>() << std::endl;

    //    for (const auto& sample: Samples) {
    //        EXPECT_NO_THROW(Dpt->FromKnx(sample.second));
    //        EXPECT_EQ(sample.first, Dpt->ToMqtt().at(0).As<std::string>());
    //    }
}

TEST_F(DptJsonTest, FromKnxToKnxTest)
{
    knx::object::TDptJsonField jsonUintField("uintField1", knx::object::TDptJsonField::EFieldType::UNSIGNED_INT, 16);
    knx::object::TDptJsonField jsonUintField2("uintField2", knx::object::TDptJsonField::EFieldType::UNSIGNED_INT, 8);
    knx::object::TDptJsonField jsonBitField("bitField", knx::object::TDptJsonField::EFieldType::BIT, 1);

    Dpt->AddField(jsonUintField, 8);
    Dpt->AddField(jsonUintField2, 24);
    Dpt->AddField(jsonBitField, 39);
    Dpt->FromKnx({0x00, 0xAA, 0xBB, 0xCC, 0x01});
    auto payload = Dpt->ToKnx();
    for (const auto& data: payload) {
        std::cout << std::hex << static_cast<uint32_t>(data) << ", ";
    }
    std::cout << std::endl;

    //    for (const auto& sample: Samples) {
    //        EXPECT_NO_THROW(Dpt->FromKnx(sample.second));
    //        EXPECT_EQ(sample.first, Dpt->ToMqtt().at(0).As<std::string>());
    //    }
}

TEST_F(DptJsonTest, fromMqttToMqttTest)
{
    knx::object::TDptJsonField jsonUintField("uintField1", knx::object::TDptJsonField::EFieldType::UNSIGNED_INT, 16);
    knx::object::TDptJsonField jsonUintField2("uintField2", knx::object::TDptJsonField::EFieldType::UNSIGNED_INT, 8);
    knx::object::TDptJsonField jsonBitField("bitField", knx::object::TDptJsonField::EFieldType::BIT, 1);

    Dpt->AddField(jsonUintField, 8);
    Dpt->AddField(jsonUintField2, 24);
    Dpt->AddField(jsonBitField, 39);
    Dpt->FromMqtt(0, R"({"bitField":false,"uintField1":45,"uintField2":56})");
    std::cout << Dpt->ToMqtt()[0].As<std::string>() << std::endl;

    //    for (const auto& sample: Samples) {
    //        EXPECT_NO_THROW(Dpt->FromKnx(sample.second));
    //        EXPECT_EQ(sample.first, Dpt->ToMqtt().at(0).As<std::string>());
    //    }
}

// TEST_F(DptJsonTest, toKnxTest)
//{
//     for (const auto& sample: Samples) {
//         EXPECT_NO_THROW(Dpt->FromMqtt(0, static_cast<double>(sample.first)));
//         EXPECT_EQ(sample.second, Dpt->ToKnx());
//     }
// }
//
// TEST_F(DptJsonTest, fromKnxNegativeTest)
//{
//     EXPECT_THROW(Dpt->FromKnx({0x00}), knx::TKnxException);
// }
//
// TEST_F(DptJsonTest, formMqttNegativeTest)
//{
//     EXPECT_THROW(Dpt->FromMqtt(0, "a"), std::exception);
// }