#include <experimental/filesystem>
#include <fstream>
#include <string>
#include <future>
#include <thread>

#include <r.h>

#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include "SerialPortDiscover.h"
#include "TestUtil.hpp"

namespace fs = std::experimental::filesystem;
using testing::UnorderedElementsAre;
using testing::Return;

bool fake_serial_ready = false;
std::mutex thread_mutex;

class SerialPortDiscover_tests : public ::testing::Test
{
protected:
	SerialPortDiscover_tests() = default;

	~SerialPortDiscover_tests() override = default;

	void SetUp() override
    {
        fake_serial_ready = false;
	}

	void TearDown() override
    {
	}

	void CreateFiles()
    {
    }

    void DeleteFiles()
    {
    }

protected:
    const std::map<std::string, std::string> device_to_json_key =
            {
                    { "Grbl", "grbl" },
                    { "BrushMotorController","brush_motors" },
                    { "RomiControlPanel",  "control_panel" }
            };

};

TEST_F(SerialPortDiscover_tests, SerialPortDiscover_can_construct)
{
    // Arrange
    // Act
    //Assert
    ASSERT_NO_THROW(SerialPortDiscover SerialPortDiscover(device_to_json_key));
}

TEST_F(SerialPortDiscover_tests, SerialPortDiscover_ConnectedDevice_bad_port_returns_empty_string)
{
    // Arrange
    SerialPortDiscover SerialPortDiscover(device_to_json_key);
    const int32_t timeout_ms = 100;
    // Act
    auto device = SerialPortDiscover.ConnectedDevice(std::string("/dev/notreal"), timeout_ms);

    //Assert
    ASSERT_TRUE(device.empty());
}

TEST_F(SerialPortDiscover_tests, SerialPortDiscover_ConnectedDevice_times_out_returns_empty_string)
{
    // Arrange
    SerialPortDiscover SerialPortDiscover(device_to_json_key);
    std::string port0 = CppLinuxSerial::TestUtil::GetInstance().GetDevice0Name();
    const int32_t timeout_ms = 100;
    // Act
    auto device = SerialPortDiscover.ConnectedDevice(port0, timeout_ms);

    //Assert
    ASSERT_TRUE(device.empty());
}

int FakeSerialDeviceFunction(const std::string& port, const std::string& device_name, std::condition_variable *cv)
{
    int result = -1;
    const int buffersize= 256;
    char buffer[buffersize];
    memset(buffer, 0, buffersize);
    serial_t * serial_port = new_serial(port.c_str(), 115200, 0);
    std::cout << "FakeSerialDeviceFunction: Notify " << std::endl;
    {
        std::lock_guard<std::mutex> lk(thread_mutex);
        fake_serial_ready = true;
        cv->notify_one();
    }

    if ( serial_port )
    {
        if (serial_read_timeout(serial_port, buffer, buffersize, 1000) == 0)
        {
            std::cout << "FakeSerialDeviceFunction: read " << buffer << std::endl;
            if (serial_write(serial_port, device_name.c_str(), device_name.length()) == 0)
            {
                result = 0;
            }
        }
    }
    delete_serial(serial_port);
    return result;
}

TEST_F(SerialPortDiscover_tests, SerialPortDiscover_ConnectedDevice_returns_when_device_not_known)
{
    // Arrange
    const int timeout_ms = 1000;
    std::condition_variable cv;
    std::string actual_device;
    std::string serial_device_name("unknown_device_name");
    std::string expected_device;
    std::string port0 = CppLinuxSerial::TestUtil::GetInstance().GetDevice0Name();
    std::string port1 = CppLinuxSerial::TestUtil::GetInstance().GetDevice1Name();

    SerialPortDiscover SerialPortDiscover(device_to_json_key);
    // NOTE: DON'T FORGET \n its canonical!
    auto future = std::async(FakeSerialDeviceFunction, port1, serial_device_name+"\n", &cv);

    // Act
    std::cout << "Wait for notify " << std::endl;
    std::unique_lock<std::mutex> lk(thread_mutex);
    cv.wait(lk, []{return fake_serial_ready == true;});
    std::cout << "received notify " << std::endl;
    actual_device = SerialPortDiscover.ConnectedDevice(port0, timeout_ms);
    int thread_success = future.get();

    // Assert
    ASSERT_EQ(expected_device, actual_device);
    ASSERT_EQ(thread_success, 0);
}

TEST_F(SerialPortDiscover_tests, SerialPortDiscover_ConnectedDevice_returns_when_device_whitespace)
{
    // Arrange
    const int timeout_ms = 1000;
    std::condition_variable cv;
    std::string actual_device;
    std::string serial_device_name(" ");
    std::string expected_device;
    std::string port0 = CppLinuxSerial::TestUtil::GetInstance().GetDevice0Name();
    std::string port1 = CppLinuxSerial::TestUtil::GetInstance().GetDevice1Name();

    SerialPortDiscover SerialPortDiscover(device_to_json_key);
    auto future = std::async(FakeSerialDeviceFunction, port1, serial_device_name+"\n", &cv);

    // Act
    std::cout << "Wait for notify " << std::endl;
    std::unique_lock<std::mutex> lk(thread_mutex);
    cv.wait(lk, []{return fake_serial_ready == true;});
    std::cout << "received notify " << std::endl;
    actual_device = SerialPortDiscover.ConnectedDevice(port0, timeout_ms);
    int thread_success = future.get();

    // Assert
    ASSERT_EQ(expected_device, actual_device);
    ASSERT_EQ(thread_success, 0);
}


TEST_F(SerialPortDiscover_tests, SerialPortDiscover_ConnectedDevice_returns_connected_device)
{
    // Arrange
    const int timeout_ms = 1000;
    std::condition_variable cv;
    std::string actual_device;
    std::string serial_device_name(device_to_json_key.begin()->first);
    std::string expected_device(device_to_json_key.begin()->second);
    std::string port0 = CppLinuxSerial::TestUtil::GetInstance().GetDevice0Name();
    std::string port1 = CppLinuxSerial::TestUtil::GetInstance().GetDevice1Name();

    SerialPortDiscover SerialPortDiscover(device_to_json_key);
    // NOTE: DON'T FORGET \n its canonical!
    auto future = std::async(FakeSerialDeviceFunction, port1, serial_device_name+"\n", &cv);

    // Act
    std::cout << "Wait for notify " << std::endl;
    std::unique_lock<std::mutex> lk(thread_mutex);
    cv.wait(lk, []{return fake_serial_ready == true;});
    std::cout << "received notify " << std::endl;
    actual_device = SerialPortDiscover.ConnectedDevice(port0, timeout_ms);
    int thread_success = future.get();

    // Assert
    ASSERT_EQ(expected_device, actual_device);
    ASSERT_EQ(thread_success, 0);
}