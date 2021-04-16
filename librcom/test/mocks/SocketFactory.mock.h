#include "gmock/gmock.h"
#include "ISocketFactory.h"

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Weffc++"

class MockSocketFactory : public rcom::ISocketFactory
{
public:
        MOCK_METHOD(std::unique_ptr<rcom::IWebSocket>, new_server_side_websocket, (int sockfd), (override));
        MOCK_METHOD(std::unique_ptr<rcom::IWebSocket>, new_client_side_websocket,
                    (rcom::IAddress& remote_address), (override));
};
