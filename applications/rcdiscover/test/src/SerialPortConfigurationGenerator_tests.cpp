#include <experimental/filesystem>
#include <fstream>
#include <string>
#include <future>

#include <r.h>

#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include "SerialPortConfigurationGenerator.h"

namespace fs = std::experimental::filesystem;
using testing::UnorderedElementsAre;
using testing::Return;

class SerialPortConfigurationGenerator_tests : public ::testing::Test
{
protected:
	SerialPortConfigurationGenerator_tests() = default;

	~SerialPortConfigurationGenerator_tests() override = default;

	void SetUp() override
    {

	}

	void TearDown() override
    {
	}

protected:

};

TEST_F(SerialPortConfigurationGenerator_tests, SerialPortConfigurationGenerator_can_construct)
{
    // Arrange
    // Act
    //Assert
    ASSERT_NO_THROW(SerialPortConfigurationGenerator SerialPortConfigurationGenerator);
}


TEST_F(SerialPortConfigurationGenerator_tests, SerialPortConfigurationGenerator_when_no_devices_present_creates_skeleton)
{
    // Arrange
    std::string json_configuration;
    std::vector<std::pair<std::string, std::string>> attached_devices;
    SerialPortConfigurationGenerator SerialPortConfigurationGenerator;
    std::string expected_json("{\n    \"ports\": {\n    }\n}");
    std::string file_name("serial_config.json");
    remove(file_name.c_str());

    // Act
    auto actual_create = SerialPortConfigurationGenerator.CreateConfigurationFile(json_configuration, attached_devices, file_name);
    auto actual = SerialPortConfigurationGenerator.LoadConfiguration(file_name);

    //Assert
    ASSERT_EQ(actual, expected_json);
    ASSERT_EQ(actual_create, 0);
}

TEST_F(SerialPortConfigurationGenerator_tests, SerialPortConfigurationGenerator_when_devices_present_creates_config)
{
    // Arrange
    std::string json_configuration;
    std::vector<std::pair<std::string, std::string>> attached_devices;
    SerialPortConfigurationGenerator SerialPortConfigurationGenerator;

    attached_devices.push_back(std::make_pair(std::string("/dev/ACM0"), std::string("brushmotorcontroller")));
    attached_devices.push_back(std::make_pair(std::string("/dev/ACM1"), std::string("cnc")));
    std::string expected_json("{\n    \"ports\": {\n        \"brushmotorcontroller\": {\n            \"port\": \"/dev/ACM0\",\n            \"type\": \"serial\"\n        },\n        \"cnc\": {\n            \"port\": \"/dev/ACM1\",\n            \"type\": \"serial\"\n        }\n    }\n}");
    std::string file_name("serial_config.json");
    remove(file_name.c_str());

    // Act
    auto actual_create = SerialPortConfigurationGenerator.CreateConfigurationFile(json_configuration, attached_devices, file_name);
    auto actual = SerialPortConfigurationGenerator.LoadConfiguration(file_name);

    //Assert
    ASSERT_EQ(actual, expected_json);
    ASSERT_EQ(actual_create, 0);
}

TEST_F(SerialPortConfigurationGenerator_tests, SerialPortConfigurationGenerator_when_existing_invalid_json_creates_new_config)
{
    // Arrange
    std::string json_configuration("This is not json data: is it? : I don't think so");
    std::vector<std::pair<std::string, std::string>> attached_devices;
    SerialPortConfigurationGenerator SerialPortConfigurationGenerator;

    attached_devices.push_back(std::make_pair(std::string("/dev/ACM0"), std::string("brushmotorcontroller")));
    attached_devices.push_back(std::make_pair(std::string("/dev/ACM1"), std::string("cnc")));
//    std::string expected_json("{\n    \"ports\": {\n        \"cnc\": {\n            \"port\": \"/dev/ACM1\",\n            \"type\": \"serial\"\n        },\n        \"brushmotorcontroller\": {\n            \"port\": \"/dev/ACM0\",\n            \"type\": \"serial\"\n        }\n    }\n}");
    std::string expected_json("{\n    \"ports\": {\n        \"brushmotorcontroller\": {\n            \"port\": \"/dev/ACM0\",\n            \"type\": \"serial\"\n        },\n        \"cnc\": {\n            \"port\": \"/dev/ACM1\",\n            \"type\": \"serial\"\n        }\n    }\n}");
    std::string file_name("serial_config.json");
    remove(file_name.c_str());

    // Act
    auto actual_create = SerialPortConfigurationGenerator.CreateConfigurationFile(json_configuration, attached_devices, file_name);
    auto actual = SerialPortConfigurationGenerator.LoadConfiguration(file_name);

    //Assert
    ASSERT_EQ(actual, expected_json);
    ASSERT_EQ(actual_create, 0);
}

TEST_F(SerialPortConfigurationGenerator_tests, SerialPortConfigurationGenerator_when_existing_valid_json_adds_to_config)
{
    // Arrange
    std::string json_configuration("{\"some_json_key\":{\"cnc\":{\"port\":\"/dev/ACM1\",\"type\":\"serial\"}}}");
    std::vector<std::pair<std::string, std::string>> attached_devices;
    SerialPortConfigurationGenerator SerialPortConfigurationGenerator;

    attached_devices.push_back(std::make_pair(std::string("/dev/ACM0"), std::string("brushmotorcontroller")));
    attached_devices.push_back(std::make_pair(std::string("/dev/ACM1"), std::string("cnc")));
//    std::string expected_json("{\n    \"some_json_key\": {\n        \"cnc\": {\n            \"port\": \"/dev/ACM1\",\n            \"type\": \"serial\"\n        }\n    },\n    \"ports\": {\n        \"cnc\": {\n            \"port\": \"/dev/ACM1\",\n            \"type\": \"serial\"\n        },\n        \"brushmotorcontroller\": {\n            \"port\": \"/dev/ACM0\",\n            \"type\": \"serial\"\n        }\n    }\n}");
    std::string expected_json("{\n    \"some_json_key\": {\n        \"cnc\": {\n            \"port\": \"/dev/ACM1\",\n            \"type\": \"serial\"\n        }\n    },\n    \"ports\": {\n        \"brushmotorcontroller\": {\n            \"port\": \"/dev/ACM0\",\n            \"type\": \"serial\"\n        },\n        \"cnc\": {\n            \"port\": \"/dev/ACM1\",\n            \"type\": \"serial\"\n        }\n    }\n}");
    std::string file_name("serial_config.json");
    remove(file_name.c_str());

    // Act
    auto actual_create = SerialPortConfigurationGenerator.CreateConfigurationFile(json_configuration, attached_devices, file_name);
    auto actual = SerialPortConfigurationGenerator.LoadConfiguration(file_name);

    //Assert
    ASSERT_EQ(actual, expected_json);
    ASSERT_EQ(actual_create, 0);
}

TEST_F(SerialPortConfigurationGenerator_tests, SerialPortConfigurationGenerator_create_fails_when_cant_write)
{
    // Arrange
    std::string json_configuration;
    std::vector<std::pair<std::string, std::string>> attached_devices;
    SerialPortConfigurationGenerator SerialPortConfigurationGenerator;
    std::string expected_json("{\n    \"ports\": {\n    }\n}");
    std::string file_name("/impossible/serial_config.json");

    // Act
    auto actual_create = SerialPortConfigurationGenerator.CreateConfigurationFile(json_configuration, attached_devices, file_name);

    //Assert
    ASSERT_NE(actual_create, 0);
}

TEST_F(SerialPortConfigurationGenerator_tests, SerialPortConfigurationGenerator_save_configuration_fails_when_cant_write)
{
    // Arrange
    SerialPortConfigurationGenerator SerialPortConfigurationGenerator;
    std::string file_contents("The quick brown fox jumped over the lazy dog.");
    std::string file_name("/impossible/serial_config.json");
    // Act
    auto actual = SerialPortConfigurationGenerator.SaveConfiguration(file_name, file_contents);

    //Assert
    ASSERT_FALSE(actual);
}

TEST_F(SerialPortConfigurationGenerator_tests, SerialPortConfigurationGenerator_save_configuration_succeeds_when_written_can_be_loaded)
{
    // Arrange
    SerialPortConfigurationGenerator SerialPortConfigurationGenerator;
    std::string expected_file_contents("The quick brown fox jumped over the lazy dog.");
    std::string file_name("serial_config.json");

    remove(file_name.c_str());

    // Act
    auto actual = SerialPortConfigurationGenerator.SaveConfiguration(file_name, expected_file_contents);
    std::string actual_file_contents = SerialPortConfigurationGenerator.LoadConfiguration(file_name);

    //Assert
    ASSERT_EQ(actual_file_contents, expected_file_contents);
    ASSERT_TRUE(actual);
}

TEST_F(SerialPortConfigurationGenerator_tests, SerialPortConfigurationGenerator_load_configuration_fails_returns_empty_string)
{
    // Arrange
    SerialPortConfigurationGenerator SerialPortConfigurationGenerator;
    std::string file_name("serial_config_noexist.json");
    remove(file_name.c_str());

    // Act
    auto actual = SerialPortConfigurationGenerator.LoadConfiguration(file_name);

    //Assert
    ASSERT_TRUE(actual.empty());
}
