#ifndef RCDISCOVER_SERIALPORTDISCOVER_H
#define RCDISCOVER_SERIALPORTDISCOVER_H

#include <map>
#include "ISerialPortDiscover.h"

class SerialPortDiscover : public ISerialPortDiscover {
    public:
        SerialPortDiscover(const std::map<std::string, std::string>& deviceFilter);
        virtual ~SerialPortDiscover() = default;
        std::string ConnectedDevice(const std::string& path, const int32_t timeout_ms) override;

    private:
        std::string FilterDevice(char* buffer);
    private:
        const std::map<std::string, std::string>& knownDevicesMap;
};

#endif //RCDISCOVER_SERIALPORTDISCOVER_H
