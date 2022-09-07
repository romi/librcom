#include "gmock/gmock.h"
#include "rcom/IAddress.h"

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Weffc++"

class MockAddress : public rcom::IAddress
{
public:
        MOCK_METHOD(void, set, (const char *ip, uint16_t port), (override));
        MOCK_METHOD(void, set, (const std::string& str), (override));
        MOCK_METHOD(void, set, (const IAddress& other), (override));
        MOCK_METHOD(bool, is_set, (), (override));
        MOCK_METHOD(std::string&, tostring, (std::string& str), (override));                
        MOCK_METHOD(struct sockaddr_in, get_sockaddr, (), (const override));
};

#pragma GCC diagnostic pop
