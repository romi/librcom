#include "SerialPortIdentification.h"

#include <filesystem>
namespace fs = std::filesystem;

SerialPortIdentification::SerialPortIdentification(ISerialPortDiscover& iserialPortDiscover)
        : serialPortDiscover(iserialPortDiscover)
{
}

void
SerialPortIdentification::ConnectedDevices(std::vector<std::string>& serialDevices,
                                           DeviceMap& devices)
{
        const int32_t timeout_ms = 500;
        for (const auto& device : serialDevices) {
                std::string deviceName;
                deviceName = serialPortDiscover.ConnectedDevice(device, timeout_ms);
                if (!deviceName.empty()) {
                        devices.emplace_back(std::make_pair(device, deviceName));
                }
        }
}
