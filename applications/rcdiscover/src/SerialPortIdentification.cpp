#include "SerialPortIdentification.h"

#include <filesystem>
namespace fs = std::filesystem;

SerialPortIdentification::SerialPortIdentification(ISerialPortDiscover& iserialPortDiscover) : serialPortDiscover(iserialPortDiscover)
{
}

std::vector<std::string>
SerialPortIdentification::ListFilesOfType(const std::string& directory, const std::string& type)
{
    std::vector<std::string> filenames;
    try
    {
        if (type.length() > 0)
        {
            for(auto& p : fs::directory_iterator(directory))
            {
                if (p.path().string().find( type ) !=std::string::npos)
                    filenames.emplace_back(p.path());
            }
        }
    }
    catch (std::exception& e)
    {
        std::cout << e.what() << std::endl;
    }
    return filenames;
}

std::vector< std::pair <std::string, std::string> >
SerialPortIdentification::ConnectedDevices(std::vector<std::string>& serialDevices)
{
    std::vector< std::pair <std::string, std::string> > devices;
    const int32_t timeout_ms = 500;
    for (const auto& device : serialDevices)
    {
        std::string deviceName = serialPortDiscover.ConnectedDevice(device, timeout_ms);
        if (!deviceName.empty())
        {
            devices.emplace_back(std::make_pair(device, deviceName));
        }
    }
    return devices;
}
