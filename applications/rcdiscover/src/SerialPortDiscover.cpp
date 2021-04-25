#include <log.h>
#include <serial_posix.h>
#include <iostream>
#include <cstring>
#include "StringUtils.h"
#include "SerialPortDiscover.h"

SerialPortDiscover::SerialPortDiscover(const std::map<std::string, std::string>& deviceFilter) : knownDevicesMap(deviceFilter)
{
}

std::string
SerialPortDiscover::ConnectedDevice(const std::string& path, const int32_t timeout_ms)
{
    const int retrycount = 3;
    int retrycurrent = 0;
    bool checked = false;
    std::string device;
    std::string command = "#?:xxxx";
    const int bufflen = 256;
    char buffer[bufflen];
    ::memset(buffer, 0, bufflen);
    serial_t * serial_port = new_serial(path.c_str(), 115200, 0);

    if ( serial_port )
    {
        while ((checked == false) && (retrycurrent++ < retrycount))
        {
            if  (serial_println(serial_port, command.c_str()) == 0)
            {
                if (serial_read_timeout(serial_port, buffer, bufflen, timeout_ms) == 0)
                {
                    std::string buffer_string(buffer);
                    std::string trimmed_device = StringUtils::trim(buffer_string, StringUtils::whitespace_chars);
                    if (!trimmed_device.empty())
                    {
                        std::cout << path << " " << buffer << std::endl;
                        device = FilterDevice(buffer);
                        checked = true;
                    }
                }
            }
        }
    }
    delete_serial(serial_port);
    return device;
}

std::string
SerialPortDiscover::FilterDevice(char* buffer)
{
    std::string buffer_string(buffer);
    for (const auto& device : knownDevicesMap)
    {
            if (buffer_string.find(device.first) != std::string::npos) {
                std::cout << "Found: " << device.second << std::endl;
                return device.second;
            }
    }
    return std::string();
}
