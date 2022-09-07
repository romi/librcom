#include "gmock/gmock.h"
#include "rcom/IResponse.h"

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Weffc++"

class MockResponse : public rcom::IResponse
{
public:        
        MOCK_METHOD(int, get_code, (), (override));
        MOCK_METHOD(void, set_code, (int code), (override));
        MOCK_METHOD(void, add_header, (const std::string& name, const std::string& value), (override));
        MOCK_METHOD(bool, get_header_value, (const std::string& name, std::string& value), (override));
        MOCK_METHOD(void, assert_websocket, (const std::string& accept), (override));
};

#pragma GCC diagnostic pop

