#include <iostream>
#include <vector>
#include <fstream>
#include <experimental/filesystem>

#include <r.h>
#include "SerialPortConfigurationGenerator.h"

SerialPortConfigurationGenerator::SerialPortConfigurationGenerator()
:
        serial_ports_configuration_key("ports"), serial_port_key("port"), serial_device_type("type"), serial_type("serial")
{

}

json_object_t SerialPortConfigurationGenerator::CreateConfigurationBase(const std::string& json_configuration)
{
    json_object_t configuration_object;
    if (json_configuration.empty())
    {
        configuration_object = json_object_create();
    } else
    {
        int parse_error = 0;
        char error_message[256];
        configuration_object = json_parse_ext(json_configuration.c_str(), &parse_error, error_message, sizeof(error_message));
        if (json_isnull(configuration_object))
        {
            configuration_object = json_object_create();
        }
    }
    return configuration_object;
}

int SerialPortConfigurationGenerator::CreateConfigurationFile(const std::string& json_configuration, const std::vector<std::pair<std::string, std::string>>& devices, const std::string& ouput_file)
{
    const int buff_size = 8192;
    char json_string_buff[buff_size];
    memset(json_string_buff, 0, buff_size);
    std::string json_string;

    json_object_t configuration_object = CreateConfigurationBase(json_configuration);
    json_object_t ports_object = json_object_create();
    json_object_set(configuration_object, serial_ports_configuration_key.c_str(), ports_object);

    for (const auto& device : devices)
    {
        json_object_t device_object = json_object_create();
        json_object_setstr(device_object, serial_device_type.c_str(), serial_type.c_str());
        json_object_setstr(device_object, serial_port_key.c_str(), device.first.c_str());
        json_object_set(ports_object, device.second.c_str(), device_object);
        json_unref(device_object);
    }

    // ToDo: Refactor to use Json_tostring, but refactor json_tostring to take flags then we will retain the exception information on file write failures.
    int retval = json_tofile(configuration_object, k_json_pretty, ouput_file.c_str());

    json_unref(configuration_object);
    json_unref(ports_object);

    return retval;
}

std::string SerialPortConfigurationGenerator::LoadConfiguration(const std::string& configuration_file) const
{
    std::ostringstream contents;
    try
    {
        std::ifstream in;
        in.exceptions(std::ifstream::badbit | std::ifstream::failbit);
        in.open(configuration_file);
        contents << in.rdbuf();
    }
    catch (const std::exception& ex)
    {
        std::cout << __PRETTY_FUNCTION__  << " failed to load file: '" << configuration_file << "' exception: " << ex.what() << std::endl;
    }

    return(contents.str());
}

bool SerialPortConfigurationGenerator::SaveConfiguration(const std::string& configuration_file, const std::string& configuration_json)
{
    bool result = true;
    try
    {
        std::ofstream out;
        out.exceptions(std::ifstream::badbit | std::ifstream::failbit);
        out.open(configuration_file);
        out << configuration_json;
        std::cout << "wrote config: " << configuration_file << std::endl;
    }
    catch (const std::exception& ex)
    {
        std::cout << __PRETTY_FUNCTION__  << " failed to write file: '" << configuration_file << "' exception: " << ex.what() << std::endl;
        result = false;
    }
    return result;
}