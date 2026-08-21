#include "../src/knxexception.h"
#include "../src/knxgroupobject/dptjson.h"
#include "testutils.h"
#include <gtest/gtest.h>

class DptJsonTest: public ::testing::Test
{
protected:
    void SetUp() override
    {
        Dpt = std::make_unique<knx::object::TDptJson>(knx::object::TDatapointId{});
    }

    void TearDown() override
    {}

    std::unique_ptr<knx::object::TDptJson> Dpt;
    std::vector<uint8_t> KnxPayload;
    std::string JsonSample;

    void ConfigureJsonDpt()
    {
        knx::object::TDptJsonField jsonUintField("uintField1",
                                                 knx::object::TDptJsonField::EFieldType::UNSIGNED_INT,
                                                 16);
        knx::object::TDptJsonField jsonUintField2("uintField2",
                                                  knx::object::TDptJsonField::EFieldType::UNSIGNED_INT,
                                                  8);
        knx::object::TDptJsonField jsonBitField("bitField", knx::object::TDptJsonField::EFieldType::BIT, 1);
        knx::object::TDptJsonField jsonFloat16Field("float16Field", knx::object::TDptJsonField::EFieldType::FLOAT, 16);
        knx::object::TDptJsonField jsonFloat32Field("float32Field", knx::object::TDptJsonField::EFieldType::FLOAT, 32);
        knx::object::TDptJsonField jsonInt8Field("int8Field", knx::object::TDptJsonField::EFieldType::INT, 8);
        knx::object::TDptJsonField jsonInt16Field("int16Field", knx::object::TDptJsonField::EFieldType::INT, 16);
        knx::object::TDptJsonField jsonInt32Field("int32Field", knx::object::TDptJsonField::EFieldType::INT, 32);

        Dpt->AddField(jsonUintField, 8);
        Dpt->AddField(jsonUintField2, 24);
        Dpt->AddField(jsonBitField, 39);
        Dpt->AddField(jsonFloat16Field, 40);
        Dpt->AddField(jsonFloat32Field, 56);
        Dpt->AddField(jsonInt8Field, 88);
        Dpt->AddField(jsonInt16Field, 96);
        Dpt->AddField(jsonInt32Field, 112);

        KnxPayload = {0x00,
                      0xAA,
                      0xBB,
                      0xCC,
                      0x01,
                      0x0D,
                      0x60,
                      0xBA,
                      0xA1,
                      0xD2,
                      0x4D,
                      0xE7,
                      0xFE,
                      0x1A,
                      0xFC,
                      0x1C,
                      0x9E,
                      0x48};

        JsonSample =
            R"({"bitField":true,"uintField1":43707,"uintField2":204,
            "float16Field":27.52,"float32Field":-0.0012346,"int8Field":-25,
            "int16Field":-486, "int32Field": -65233336})";
    }

    void ConfigureJsonDptString()
    {
        knx::object::TDptJsonField jsonCharField("charField", knx::object::TDptJsonField::EFieldType::CHAR, 8);
        knx::object::TDptJsonField jsonStringField("stringField", knx::object::TDptJsonField::EFieldType::CHAR, 112);

        Dpt->AddField(jsonCharField, 8);
        Dpt->AddField(jsonStringField, 16);

        KnxPayload = {0x00, 'p', 'H', 'e', 'l', 'l', 'o', ',', ' ', 'w', 'o', 'r', 'l', 'd', 0x00, 0x00};

        JsonSample = R"({"charField": "p", "stringField": "Hello, world"})";
    }
};

TEST_F(DptJsonTest, toMqttTest)
{
    ConfigureJsonDpt();

    Dpt->FromKnx(KnxPayload);
    auto jsonStr = Dpt->ToMqtt()[0].As<std::string>();

    auto jsonObject = testUtils::ParseJson(jsonStr);

    EXPECT_EQ(true, jsonObject["bitField"].asBool());
    EXPECT_EQ(0xAABB, jsonObject["uintField1"].asUInt());
    EXPECT_EQ(0xCC, jsonObject["uintField2"].asUInt());
    EXPECT_NEAR(27.52, jsonObject["float16Field"].asFloat(), 0.01);
    EXPECT_NEAR(-0.0012346, jsonObject["float32Field"].asFloat(), 0.0000001);
    EXPECT_EQ(-25, jsonObject["int8Field"].asInt());
    EXPECT_EQ(-486, jsonObject["int16Field"].asInt());
    EXPECT_EQ(-65233336, jsonObject["int32Field"].asInt());
}

TEST_F(DptJsonTest, toMqttStringFieldTest)
{
    ConfigureJsonDptString();

    Dpt->FromKnx(KnxPayload);
    auto jsonStr = Dpt->ToMqtt()[0].As<std::string>();

    auto jsonObject = testUtils::ParseJson(jsonStr);

    EXPECT_EQ("p", jsonObject["charField"].asString());
    EXPECT_EQ("Hello, world", jsonObject["stringField"].asString());
}

TEST_F(DptJsonTest, toMqttUnsigned32FieldTest)
{
    knx::object::TDptJsonField jsonUint32Field("uint32Field", knx::object::TDptJsonField::EFieldType::UNSIGNED_INT, 32);

    // Bit position 8 is where TDptJsonBuilder places a "U32" encoding: the field is wider
    // than 6 bits, so the first payload octet stays blank and the value starts at octet 1.
    // That makes the payload 5 octets long, most significant octet first.
    Dpt->AddField(jsonUint32Field, 8);

    struct TUnsigned32Case
    {
        std::vector<uint8_t> Payload;
        uint64_t ExpectedValue;
    };

    // Every case except the all-zero one decoded to 0 before the fix.
    const std::vector<TUnsigned32Case> testCases = {
        {{0x00, 0x00, 0x00, 0x00, 0x00}, 0x00000000},
        {{0x00, 0x7F, 0xFF, 0xFF, 0xFF}, 0x7FFFFFFF}, // largest value with bit 31 clear
        {{0x00, 0x80, 0x00, 0x00, 0x00}, 0x80000000}, // bit 31 on its own
        {{0x00, 0xDE, 0xAD, 0xBE, 0xEF}, 0xDEADBEEF}, // bit 31 set among others
        {{0x00, 0xFF, 0xFF, 0xFF, 0xFF}, 0xFFFFFFFF}  // every bit set
    };

    for (const auto& testCase: testCases) {
        SCOPED_TRACE("expected value " + std::to_string(testCase.ExpectedValue));

        Dpt->FromKnx(testCase.Payload);

        auto jsonObject = testUtils::ParseJson(Dpt->ToMqtt()[0].As<std::string>());
        EXPECT_EQ(testCase.ExpectedValue, jsonObject["uint32Field"].asUInt64());

        // The value must also survive the way back, so that a full-width field is not
        // truncated on its way to a KNX telegram.
        EXPECT_EQ(testCase.Payload, Dpt->ToKnx());
    }
}

TEST_F(DptJsonTest, stringFieldTestNeg)
{
    ConfigureJsonDptString();
    Dpt->FromMqtt(0, R"({"charField": "p", "stringField": "0123456789abcdef"})");

    KnxPayload = {0x00, 'p', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd'};
    EXPECT_EQ(KnxPayload, Dpt->ToKnx());
}

TEST_F(DptJsonTest, toKnxTest)
{
    ConfigureJsonDpt();

    Dpt->FromMqtt(0, JsonSample);

    EXPECT_EQ(KnxPayload, Dpt->ToKnx());
}

TEST_F(DptJsonTest, toKnxStringFieldTest)
{
    ConfigureJsonDptString();

    Dpt->FromMqtt(0, JsonSample);

    EXPECT_EQ(KnxPayload, Dpt->ToKnx());
}

TEST_F(DptJsonTest, toKnxNonAsciiStringFieldTest)
{
    knx::object::TDptJsonField jsonStringField("stringField", knx::object::TDptJsonField::EFieldType::CHAR, 112);

    // A lone A112 field starts at octet 1, the way TDptJsonBuilder lays out DPT 16.
    Dpt->AddField(jsonStringField, 8);

    // "Kuche" with an umlaut, UTF-8 encoded as the web UI sends it: U+00FC is 0xC3 0xBC.
    // The literal is split so the hex escape does not swallow the following 'c'.
    const std::string utf8Text = "K\xC3\xBC"
                                 "che";

    // Blank first octet, then the text most significant octet first, zero padded to 14.
    const std::vector<uint8_t> expectedPayload =
        {0x00, 0x4B, 0xC3, 0xBC, 0x63, 0x68, 0x65, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

    // MQTT -> KNX: the direction the signed char broke.
    Dpt->FromMqtt(0, std::string{R"({"stringField":")"} + utf8Text + R"("})");
    EXPECT_EQ(expectedPayload, Dpt->ToKnx());

    // KNX -> MQTT: this direction extracts octets through to_ulong() and was never affected,
    // but assert it too so the round trip stays lossless for non-ASCII text.
    Dpt->FromKnx(expectedPayload);
    auto jsonObject = testUtils::ParseJson(Dpt->ToMqtt()[0].As<std::string>());
    EXPECT_EQ(utf8Text, jsonObject["stringField"].asString());
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