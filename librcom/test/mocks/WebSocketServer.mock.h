#include "gmock/gmock.h"
#include "rcom/IWebSocketServer.h"

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Weffc++"

class MockWebSocketServer : public rcom::IWebSocketServer
{
public:
        MOCK_METHOD(void, handle_events, (), (override));
        MOCK_METHOD(void, broadcast, (rcom::MemBuffer &message, rcom::MessageType type, rcom::IWebSocket *exclude), (override));
        MOCK_METHOD(void, get_address, (rcom::IAddress& address), (override));
        MOCK_METHOD(size_t, count_links, (), (override));
        MOCK_METHOD(rcom::IWebSocket&, get_link, (size_t index), (override));
};
#pragma GCC diagnostic pop
