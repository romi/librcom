#include "gmock/gmock.h"
#include "IMessageListener.h"

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Weffc++"

class MockMessageListener : public rcom::IMessageListener
{
public:
        MOCK_METHOD(void, onmessage, (rcom::IWebSocket& link, rcom::MemBuffer& message, rcom::MessageType type), (override));
};
#pragma GCC diagnostic pop
