#ifndef RCDISCOVER_ISERIALPORT_DISCOVER_H
#define RCDISCOVER_ISERIALPORT_DISCOVER_H

class ISerialPortDiscover {
    public:
        virtual ~ISerialPortDiscover() = default;
        virtual std::string ConnectedDevice(const std::string& path, const int32_t timeout_ms) = 0;
};
#endif //RCDISCOVER_ISERIALPORT_DISCOVER_H
