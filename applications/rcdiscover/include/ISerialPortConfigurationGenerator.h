#ifndef ROMI_ROVER_BUILD_AND_TEST_ISERIALPORTCONFIGURATIONGENERATOR_H
#define ROMI_ROVER_BUILD_AND_TEST_ISERIALPORTCONFIGURATIONGENERATOR_H

#include <string>
#include <vector>

class ISerialPortConfigurationGenerator {
    public:
        ISerialPortConfigurationGenerator() = default;
        virtual ~ISerialPortConfigurationGenerator() = default;
        virtual int CreateConfigurationFile(const std::string& json_configuration, const std::vector<std::pair<std::string, std::string>>& devices, const std::string& ouput_file) = 0;
        virtual std::string LoadConfiguration(const std::string& configuration_file) const = 0;
        virtual bool SaveConfiguration(const std::string& configuration_file, const std::string& configuration_json) = 0;
};


#endif //ROMI_ROVER_BUILD_AND_TEST_SERIALPORTCONFIGURATIONGENERATOR_H
