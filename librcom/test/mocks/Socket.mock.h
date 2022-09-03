#include "gmock/gmock.h"
#include "rcom/ISocket.h"

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Weffc++"

class MockSocket : public rcom::ISocket
{
public:
        MOCK_METHOD(void, close, (), (override));
        MOCK_METHOD(bool, is_connected, (), (override));
        MOCK_METHOD(bool, is_endpoint_connected, (), (override));
        MOCK_METHOD(bool, send, (rcom::MemBuffer& buffer), (override));
        MOCK_METHOD(bool, send, (const uint8_t *buffer, size_t length), (override));
        MOCK_METHOD(bool, read, (uint8_t *buffer, size_t length), (override));
        MOCK_METHOD(rcom::WaitStatus, wait, (double timeout_in_seconds), (override));
        MOCK_METHOD(void, turn_buffering_off, (), (override));
        MOCK_METHOD(void, turn_buffering_on, (), (override));
        MOCK_METHOD(rcom::ILinux&, get_linux, (), (override));
};

#pragma GCC diagnostic pop
