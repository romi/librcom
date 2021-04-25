#ifndef RCDISCOVER_PORTCONFIGURATIONGENERATOR_H
#define RCDISCOVER_PORTCONFIGURATIONGENERATOR_H

#include <string>
#include <vector>
#include "IPortConfigurationGenerator.h"
#include <json.h>

class PortConfigurationGenerator : public IPortConfigurationGenerator
{
public:
        PortConfigurationGenerator();
        virtual ~PortConfigurationGenerator() = default;
        
        int CreateConfigurationFile(const std::string& json_configuration,
                                    const DeviceMap& devices,
                                    const std::string& ouput_file) override;
        std::string LoadConfiguration(const std::string& configuration_file) const override;
        bool SaveConfiguration(const std::string& configuration_file,
                               const std::string& configuration_json) override;
private:
        json_object_t CreateConfigurationBase(const std::string& json_configuration);
private:
        const std::string serial_ports_configuration_key;
        const std::string serial_port_key;
        const std::string serial_device_type;
        const std::string serial_type;
};

#endif //RCDISCOVER_PORTCONFIGURATIONGENERATOR_H
