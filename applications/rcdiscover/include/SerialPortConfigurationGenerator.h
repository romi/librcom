#ifndef ROMI_ROVER_BUILD_AND_TEST_SERIALPORTCONFIGURATIONGENERATOR_H
#define ROMI_ROVER_BUILD_AND_TEST_SERIALPORTCONFIGURATIONGENERATOR_H

#include <string>
#include <vector>
#include "ISerialPortConfigurationGenerator.h"
#include <json.h>

class SerialPortConfigurationGenerator : public ISerialPortConfigurationGenerator
{
public:
        SerialPortConfigurationGenerator();
        virtual ~SerialPortConfigurationGenerator() = default;
        int CreateConfigurationFile(const std::string& json_configuration, const std::vector<std::pair<std::string, std::string>>& devices, const std::string& ouput_file) override;
        std::string LoadConfiguration(const std::string& configuration_file) const override;
        bool SaveConfiguration(const std::string& configuration_file, const std::string& configuration_json) override;
private:
        json_object_t CreateConfigurationBase(const std::string& json_configuration);        const std::string serial_ports_configuration_key;
private:
        const std::string serial_port_key;
        const std::string serial_device_type;
        const std::string serial_type;
};


#endif //ROMI_ROVER_BUILD_AND_TEST_SERIALPORTCONFIGURATIONGENERATOR_H
