#include <unistd.h>
#include <string>
#include <iostream>
#include <stdexcept>
#include "rcom/IRPCHandler.h"
#include "rcom/RcomServer.h"
#include "HappyMonster.h"

class MonsterAdaptor : public rcom::IRPCHandler
{
protected:
        IMonster& monster_;

        void execute_jump_around();
        void execute_gently_scare_someone(nlohmann::json& params);
        void execute_get_energy_level(nlohmann::json& result);
                
public:
        MonsterAdaptor(IMonster& monster);
        ~MonsterAdaptor() override = default;
        
        void execute(const std::string& method, nlohmann::json& params,
                     nlohmann::json& result, rcom::RPCError& status) override;
        void execute(const std::string& method, nlohmann::json& params,
                     rcom::MemBuffer& result, rcom::RPCError &status) override;
};

MonsterAdaptor::MonsterAdaptor(IMonster& monster)
        : monster_(monster)
{
}

void MonsterAdaptor::execute(const std::string& method, nlohmann::json& params,
                             nlohmann::json& result, rcom::RPCError& error)
{
        error.code = 0;
        if (method == "jump-around") {
                execute_jump_around();
                
        } else if (method == "gently-scare-someone") {
                execute_gently_scare_someone(params);
                
        } else if (method == "get-energy-level") {
                execute_get_energy_level(result);
                
        } else {
                error.code = rcom::RPCError::kMethodNotFound;
                error.message = "Unknown method";
        }
}

void MonsterAdaptor::execute(const std::string& method, nlohmann::json& params,
                             rcom::MemBuffer& result, rcom::RPCError &error)
{
        (void) method;
        (void) params;
        (void) result;
        error.code = 1;
        error.message = "Monsters don't take binary messages.";
}

void MonsterAdaptor::execute_jump_around()
{
        monster_.jump_around();
}

void MonsterAdaptor::execute_gently_scare_someone(nlohmann::json& params)
{
        std::string id = params["person-id"];
        monster_.gently_scare_someone(id);
}

void MonsterAdaptor::execute_get_energy_level(nlohmann::json& result)
{
        result["energy-level"] = monster_.get_energy_level();
}

int main()
{
        try {
                std::string name = "elmo";
                HappyMonster monster(name);
                MonsterAdaptor adaptor(monster);
                auto monster_server = rcom::RcomServer::create(name, adaptor);

                while (true) {
                        monster_server->handle_events();
                        usleep(1000);
                }
        } catch (std::exception& e) {
                std::cout << "main: caught exception: " << e.what() << std::endl;
        }
        return 0;
}
