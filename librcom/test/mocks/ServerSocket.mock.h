#include "gmock/gmock.h"
#include "IServerSocket.h"

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Weffc++"

class MockServerSocket : public rcom::IServerSocket
{
public:
        MOCK_METHOD(int, accept, (double timeout_in_seconds), (override));
        MOCK_METHOD(void, get_address, (rcom::IAddress& address), (override));
        MOCK_METHOD(void, close, (), (override));
};
