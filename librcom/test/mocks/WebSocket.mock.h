#include "gmock/gmock.h"
#include "IWebSocket.h"

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Weffc++"

class MockWebSocket : public rcom::IWebSocket
{
public:
        MOCK_METHOD(rcom::RecvStatus, recv, (rcom::MemBuffer& message, double timeout), (override));
        MOCK_METHOD(bool, send, (rcom::MemBuffer& message,
                                 rcom::MessageType type), (override));                
        MOCK_METHOD(void, close, (rcom::CloseCode code), (override));
        MOCK_METHOD(bool, is_connected, (), (override));
        MOCK_METHOD(rcom::ILinux&, get_linux, (), (override));
};

#pragma GCC diagnostic pop
