#include "gmock/gmock.h"
#include "rcom/ILog.h"

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Weffc++"

class MockLog : public rcom::ILog
{
public:
        MOCK_METHOD(void, error, (const std::string& message), (override));
        MOCK_METHOD(void, warn, (const std::string& message), (override));
        MOCK_METHOD(void, info, (const std::string& message), (override));
        MOCK_METHOD(void, debug, (const std::string& message), (override));
};

#pragma GCC diagnostic pop
