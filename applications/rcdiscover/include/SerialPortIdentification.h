#ifndef RCDISCOVER_SERIALPORTIDENTIFICATION_H
#define RCDISCOVER_SERIALPORTIDENTIFICATION_H

#include <iostream>
#include <vector>
#include "ISerialPortDiscover.h"
#include "ISerialPortIdentification.h"

class SerialPortIdentification : ISerialPortIdentification {
    public:
        SerialPortIdentification(ISerialPortDiscover& serialPortDiscover);
        virtual ~SerialPortIdentification() = default;
        std::vector<std::string> ListFilesOfType(const std::string& directory, const std::string& type) override;
        std::vector<std::pair<std::string, std::string>> ConnectedDevices(std::vector<std::string>& serialDevices) override;
    private:
    ISerialPortDiscover& serialPortDiscover;
};

#endif //RCDISCOVER_SERIALPORTIDENTIFICATION_H
