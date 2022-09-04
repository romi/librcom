#include <iostream>
#include "IMonster.h"

class HappyMonster : public IMonster
{
protected:
        std::string name_;
        double energy_;
public:
        HappyMonster(const std::string name)
                : name_(name), energy_(1.0) {
        }

        ~HappyMonster() override = default;
        
        void jump_around() override {
                std::cout << "Jump around!" << std::endl;
        }

        void gently_scare_someone(const std::string& person_id) override {
                std::cout << "Hey " << person_id
                          << ", don't watch that. Watch this. " 
                          << "This is the happy happy monster show."
                          << std::endl;
        }

        double get_energy_level() override {
                return energy_;
        }
};
