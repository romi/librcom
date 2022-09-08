#include "gmock/gmock.h"
#include "rcom/IRPCClient.h"

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Weffc++"
class MockRPCClient : public rcom::IRPCClient
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
        MOCK_METHOD(bool, is_connected, (), (override));
        MOCK_METHOD(const std::shared_ptr<rcom::ILog>&, log, (), (override));
};
#pragma GCC diagnostic pop
