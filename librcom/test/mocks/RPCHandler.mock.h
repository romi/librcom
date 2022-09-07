#include "gmock/gmock.h"
#include "rcom/IRPCHandler.h"

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Weffc++"
class MockRPCHandler : public rcom::IRPCHandler
{
public:
        MOCK_METHOD(void, execute, (const std::string& method,
                                    nlohmann::json& params,
                                    nlohmann::json& result,
                                    rcom::RPCError& error), (override));
        MOCK_METHOD(void, execute, (const std::string& method,
                                    nlohmann::json& params,
                                    rcom::MemBuffer& result,
                                    rcom::RPCError& error), (override));
};
#pragma GCC diagnostic pop
