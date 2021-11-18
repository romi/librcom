#include "gmock/gmock.h"
#include "IWebSocketServerFactory.h"

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Weffc++"

class MockWebSocketServerFactory : public rcom::IWebSocketServerFactory
{
public:
        MOCK_METHOD(std::unique_ptr<rcom::IWebSocketServer>, new_web_socket_server, (const std::shared_ptr<rcom::IMessageListener> &listener, uint16_t port), (override));
};
#pragma GCC diagnostic pop