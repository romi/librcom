#include "gmock/gmock.h"
#include "rcom/IRequest.h"

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Weffc++"

class MockRequest : public rcom::IRequest
{
public:
        MOCK_METHOD(Method, get_method, (), (override));                
        MOCK_METHOD(void, set_method, (Method method), (override));                
        MOCK_METHOD(std::string&, get_uri, (), (override));
        MOCK_METHOD(void, set_uri, (const std::string& uri), (override));
        MOCK_METHOD(void, add_header, (const std::string& name, const std::string& value), (override));
        MOCK_METHOD(bool, get_header_value, (const std::string& name, std::string& value), (override));                
        MOCK_METHOD(void, assert_websocket, (), (override));
};

#pragma GCC diagnostic pop

