#include <string>
#include <iostream>
#include <stdexcept>
#include "rcom/RemoteStub.h"
#include "rcom/RcomClient.h"
#include "rcom/ConsoleLogger.h"
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

int main()
{
        try {
                auto client = rcom::RcomClient::create("elmo", 10.0);
                RemoteMonster monster(client);        
                monster.gently_scare_someone("you");
        } catch (std::exception& e) {
                std::cout << "main: caught exception: " << e.what() << std::endl;
        }
        return 0;
}
