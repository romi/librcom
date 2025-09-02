#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "rcom/ISystem.h"

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Weffc++"

class MockLinux : public rcom::ISystem {
public:
        MOCK_METHOD(double, time, (), (override));
        MOCK_METHOD(void, sleep, (double), (override));
        MOCK_METHOD(std::string, local_ip, (), (override));
        MOCK_METHOD(void, set_local_ip, (const std::string&), (override));
        MOCK_METHOD(int, tcp_server_socket, (const rcom::IAddress&), (override));
        MOCK_METHOD(int, accept, (int), (override));
        MOCK_METHOD(int, tcp_client_socket, (const rcom::IAddress&), (override));
        MOCK_METHOD(ssize_t, recv, (int, void *, size_t, int), (override));
        MOCK_METHOD(ssize_t, send, (int, const void *, size_t), (override));
        MOCK_METHOD(int, socket_close, (int), (override));
        MOCK_METHOD(int, getaddress, (int, rcom::IAddress&), (override));                
        MOCK_METHOD(int, setsockopt, (int, int, int, const void *, int), (override));
        MOCK_METHOD(ssize_t, getrandom, (void *, size_t, unsigned int), (override));
        MOCK_METHOD(rcom::WaitStatus, wait, (int, int), (override));
};

#pragma GCC diagnostic pop
