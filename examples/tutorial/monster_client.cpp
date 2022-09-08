#include <string>
#include <iostream>
#include <stdexcept>
#include <rcom/RemoteStub.h>
#include <rcom/RcomClient.h>
#include <rcom/ILog.h>
#include "IMonster.h"

class RemoteMonster : public IMonster, public rcom::RemoteStub
{
public:
        RemoteMonster(std::unique_ptr<rcom::IRPCClient>& client);
        ~RemoteMonster() override = default;
        void jump_around() override;
        void gently_scare_someone(const std::string& person_id) override;
        double get_energy_level() override;
};

RemoteMonster::RemoteMonster(std::unique_ptr<rcom::IRPCClient>& client)
        : RemoteStub(client)
{
}

void RemoteMonster::jump_around()
{
        bool success = execute_simple_request("jump-around");
        if (!success) {
                std::cout << "jump_around failed" << std::endl;
        }
}

void RemoteMonster::gently_scare_someone(const std::string& person_id)
{
        nlohmann::json params;
        params["person-id"] = person_id;
        
        bool success = execute_with_params("gently-scare-someone", params);
        if (!success) {
                std::cout << "gently_scare_someone failed" << std::endl;
        }
}

double RemoteMonster::get_energy_level()
{
        double energy_level = -1.0;
        nlohmann::json result;
        
        bool success = execute_with_result("get-energy-level", result);
        if (success) {
                energy_level = result["energy-level"];
        } else {
                std::cout << "get_energy_level failed" << std::endl;
        }
        
        return energy_level;
}

class MyLog : public rcom::ILog
{
public:
        MyLog() {}
        ~MyLog() override = default;
                
        void error(const std::string& message) override {
                std::cout << "MyErr: " << message << std::endl;
        }
                
        void warn(const std::string& message) override {
                std::cout << "MyWarn: " << message << std::endl;
        }
                        
        void info(const std::string& message) override {
                std::cout << "MyInfo: " << message << std::endl;
        }
                
        void debug(const std::string& message) override {
                std::cout << "MyDebug: " << message << std::endl;
        }
};

int main()
{
        try {
                auto log = std::make_shared<MyLog>();
                auto client = rcom::RcomClient::create("elmo", 10.0, log);
                RemoteMonster monster(client);        
                monster.gently_scare_someone("you");
        } catch (std::exception& e) {
                std::cout << "main: caught exception: " << e.what() << std::endl;
        }
        return 0;
}
