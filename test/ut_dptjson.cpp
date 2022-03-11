#include "../src/knxexception.h"
#include "../src/knxgroupobject/dptjson.h"
#include "testutils.h"
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
};

TEST_F(DptJsonTest, toMqttTest)
{
    knx::object::TDptJsonField jsonUintField("uintField1", knx::object::TDptJsonField::EFieldType::UNSIGNED_INT, 16);
    knx::object::TDptJsonField jsonUintField2("uintField2", knx::object::TDptJsonField::EFieldType::UNSIGNED_INT, 8);
    knx::object::TDptJsonField jsonBitField("bitField", knx::object::TDptJsonField::EFieldType::BIT, 1);

    Dpt->AddField(jsonUintField, 8);
    Dpt->AddField(jsonUintField2, 24);
    Dpt->AddField(jsonBitField, 39);
    Dpt->FromKnx({0x00, 0xAA, 0xBB, 0xCC, 0x01});
    auto jsonStr = Dpt->ToMqtt()[0].As<std::string>();

    auto jsonObject = testUtils::ParseJson(jsonStr);
    EXPECT_EQ(true, jsonObject["bitField"].asBool());
    EXPECT_EQ(0xAABB, jsonObject["uintField1"].asUInt());
    EXPECT_EQ(0xCC, jsonObject["uintField2"].asUInt());
}

TEST_F(DptJsonTest, FromKnxToKnxTest)
{
    std::vector<uint8_t> knxPayload = {0x00, 0xAA, 0xBB, 0xCC, 0x01};
    knx::object::TDptJsonField jsonUintField("uintField1", knx::object::TDptJsonField::EFieldType::UNSIGNED_INT, 16);
    knx::object::TDptJsonField jsonUintField2("uintField2", knx::object::TDptJsonField::EFieldType::UNSIGNED_INT, 8);
    knx::object::TDptJsonField jsonBitField("bitField", knx::object::TDptJsonField::EFieldType::BIT, 1);

    Dpt->AddField(jsonUintField, 8);
    Dpt->AddField(jsonUintField2, 24);
    Dpt->AddField(jsonBitField, 39);
    Dpt->FromKnx(knxPayload);

    EXPECT_EQ(knxPayload, Dpt->ToKnx());
}

TEST_F(DptJsonTest, FromKnxNegTest)
{
    std::vector<uint8_t> knxPayload = {0x00, 0xAA, 0xBB, 0xCC};
    knx::object::TDptJsonField jsonUintField("uintField1", knx::object::TDptJsonField::EFieldType::UNSIGNED_INT, 16);
    knx::object::TDptJsonField jsonUintField2("uintField2", knx::object::TDptJsonField::EFieldType::UNSIGNED_INT, 8);
    knx::object::TDptJsonField jsonBitField("bitField", knx::object::TDptJsonField::EFieldType::BIT, 1);

    Dpt->AddField(jsonUintField, 8);
    Dpt->AddField(jsonUintField2, 24);
    Dpt->AddField(jsonBitField, 39);
    EXPECT_THROW(Dpt->FromKnx(knxPayload), std::exception);
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

    auto jsonStr = Dpt->ToMqtt()[0].As<std::string>();

    auto jsonObject = testUtils::ParseJson(jsonStr);
    EXPECT_EQ(false, jsonObject["bitField"].asBool());
    EXPECT_EQ(45, jsonObject["uintField1"].asUInt());
    EXPECT_EQ(56, jsonObject["uintField2"].asUInt());
}

TEST_F(DptJsonTest, fromMqttNegTest)
{
    knx::object::TDptJsonField jsonUintField("uintField1", knx::object::TDptJsonField::EFieldType::UNSIGNED_INT, 16);
    knx::object::TDptJsonField jsonUintField2("uintField2", knx::object::TDptJsonField::EFieldType::UNSIGNED_INT, 8);
    knx::object::TDptJsonField jsonBitField("bitField", knx::object::TDptJsonField::EFieldType::BIT, 1);

    Dpt->AddField(jsonUintField, 8);
    Dpt->AddField(jsonUintField2, 24);
    Dpt->AddField(jsonBitField, 39);
    EXPECT_THROW(Dpt->FromMqtt(0, R"({"bitFieldFalse":false,"uintField1":45,"uintField2":56})"), std::exception);
}