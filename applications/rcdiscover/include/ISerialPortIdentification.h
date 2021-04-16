#ifndef RCDISCOVER_ISERIALPORTIDENTIFICATION_H
#define RCDISCOVER_ISERIALPORTIDENTIFICATION_H

class ISerialPortIdentification {
    public:
        virtual ~ISerialPortIdentification() = default;
        virtual std::vector<std::string> ListFilesOfType(const std::string& directory, const std::string& type) = 0;
        virtual std::vector<std::pair<std::string, std::string>> ConnectedDevices(std::vector<std::string>& serialDevices) = 0;
};
#endif //RCDISCOVER_ISERIALPORTIDENTIFICATION_H
