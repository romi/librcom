#include <iostream>
#include "gtest/gtest.h"

#include "Address.mock.h"
#include "mock_linux.h"

#include "ServerSocket.h"
#include "Address.h"


using namespace std;
using namespace rcom;
using namespace rpp;

using ::testing::Return;
using ::testing::ReturnRef;
using ::testing::_;
using ::testing::SetArgReferee;
using ::testing::NiceMock;
using ::testing::Assign;
using ::testing::ReturnPointee;
using ::testing::DoAll;

class serversocket_tests : public ::testing::Test
{
protected:
        MockAddress address_;
        struct sockaddr_in sockaddr_;
        
        serversocket_tests() : address_(), sockaddr_() {
                memset(&sockaddr_, 0, sizeof(struct sockaddr_in));
        }

        ~serversocket_tests() override = default;

        void SetUp() override {
        }

        void TearDown() override {
        }
};

TEST_F(serversocket_tests, successfull_creation)
{
        // Arrange
        std::shared_ptr<MockLinux> mock_linux = std::make_shared<MockLinux>();
        EXPECT_CALL(address_, get_sockaddr())
                .WillOnce(Return(sockaddr_));
        EXPECT_CALL(*mock_linux, socket(_,_,_))
                .WillOnce(Return(1));
        EXPECT_CALL(*mock_linux, bind(_,_,_))
                .WillOnce(Return(0));
        EXPECT_CALL(*mock_linux, listen(_,_))
                .WillOnce(Return(0));        
        EXPECT_CALL(*mock_linux, shutdown(_,_))
                .WillOnce(Return(0));
        EXPECT_CALL(*mock_linux, recv(_,_,_,_))
                .WillOnce(Return(-1));
        EXPECT_CALL(*mock_linux, close(_))
                .WillOnce(Return(0));

        // Act
        {
                std::shared_ptr<rpp::ILinux> linux = std::move(mock_linux);
                ServerSocket socket(linux, address_);
        }
        
        // Assert
}

TEST_F(serversocket_tests, close_only_called_once)
{
        // Arrange
        std::shared_ptr<MockLinux> mock_linux = std::make_shared<MockLinux>();
        EXPECT_CALL(address_, get_sockaddr())
                .WillOnce(Return(sockaddr_));
        EXPECT_CALL(*mock_linux, socket(_,_,_))
                .WillOnce(Return(1));
        EXPECT_CALL(*mock_linux, bind(_,_,_))
                .WillOnce(Return(0));
        EXPECT_CALL(*mock_linux, listen(_,_))
                .WillOnce(Return(0));
        
        EXPECT_CALL(*mock_linux, shutdown(_,_))
                .WillOnce(Return(0));
        EXPECT_CALL(*mock_linux, recv(_,_,_,_))
                .WillOnce(Return(-1));
        EXPECT_CALL(*mock_linux, close(_))
                .WillOnce(Return(0));

        // Act
        {
                std::shared_ptr<rpp::ILinux> linux = std::move(mock_linux);
                ServerSocket socket(linux, address_);
                socket.close();
        }
        
        // Assert
}

TEST_F(serversocket_tests, failed_socket_creation_throws_exception)
{
        // Arrange
        std::shared_ptr<MockLinux> mock_linux = std::make_shared<MockLinux>();
        EXPECT_CALL(address_, get_sockaddr())
                .WillOnce(Return(sockaddr_));
        EXPECT_CALL(*mock_linux, socket(_,_,_))
                .WillOnce(Return(-1));
        
        // Act
        try {
                std::shared_ptr<rpp::ILinux> linux = std::move(mock_linux);
                ServerSocket socket(linux, address_);
                FAIL() << "Expected std::runtime_error";
        } catch(std::runtime_error const & err) {
                // OK
                
        } catch(...) {
                FAIL() << "Expected std::runtime_error";
        }
        
        // Assert
}

TEST_F(serversocket_tests, failed_bind_throws_exception)
{
        // Arrange
        std::shared_ptr<MockLinux> mock_linux = std::make_shared<MockLinux>();
        EXPECT_CALL(address_, get_sockaddr())
                .WillOnce(Return(sockaddr_));
        EXPECT_CALL(*mock_linux, socket(_,_,_))
                .WillOnce(Return(1));
        EXPECT_CALL(*mock_linux, bind(_,_,_))
                .WillOnce(Return(-1));

        EXPECT_CALL(*mock_linux, shutdown(_,_))
                .WillOnce(Return(0));
        EXPECT_CALL(*mock_linux, recv(_,_,_,_))
                .WillOnce(Return(-1));
        EXPECT_CALL(*mock_linux, close(_))
                .WillOnce(Return(0));
        
        // Act
        try {
                std::shared_ptr<rpp::ILinux> linux = std::move(mock_linux);
                ServerSocket socket(linux, address_);
                FAIL() << "Expected std::runtime_error";
        } catch(std::runtime_error const & err) {
                // OK
                
        } catch(...) {
                FAIL() << "Expected std::runtime_error";
        }
        
        // Assert
}

TEST_F(serversocket_tests, failed_listen_throws_exception)
{
        // Arrange
        std::shared_ptr<MockLinux> mock_linux = std::make_shared<MockLinux>();
        EXPECT_CALL(address_, get_sockaddr())
                .WillOnce(Return(sockaddr_));
        EXPECT_CALL(*mock_linux, socket(_,_,_))
                .WillOnce(Return(1));
        EXPECT_CALL(*mock_linux, bind(_,_,_))
                .WillOnce(Return(0));
        EXPECT_CALL(*mock_linux, listen(_,_))
                .WillOnce(Return(-1));

        EXPECT_CALL(*mock_linux, shutdown(_,_))
                .WillOnce(Return(0));
        EXPECT_CALL(*mock_linux, recv(_,_,_,_))
                .WillOnce(Return(-1));
        EXPECT_CALL(*mock_linux, close(_))
                .WillOnce(Return(0));
        
        // Act
        try {
                std::shared_ptr<rpp::ILinux> linux = std::move(mock_linux);
                ServerSocket socket(linux, address_);
                FAIL() << "Expected std::runtime_error";
        } catch(std::runtime_error const & err) {
                // OK
                
        } catch(...) {
                FAIL() << "Expected std::runtime_error";
        }
        
        // Assert
}

ACTION(SetREvent) { arg0[0].revents = POLLIN; }

TEST_F(serversocket_tests, accept_returns_expected_socket)
{
        // Arrange
        std::shared_ptr<MockLinux> mock_linux = std::make_shared<MockLinux>();
        EXPECT_CALL(address_, get_sockaddr())
                .WillOnce(Return(sockaddr_));
        EXPECT_CALL(*mock_linux, socket(_,_,_))
                .WillOnce(Return(1));
        EXPECT_CALL(*mock_linux, bind(_,_,_))
                .WillOnce(Return(0));
        EXPECT_CALL(*mock_linux, listen(_,_))
                .WillOnce(Return(0));
        
        EXPECT_CALL(*mock_linux, shutdown(_,_))
                .WillOnce(Return(0));
        EXPECT_CALL(*mock_linux, recv(_,_,_,_))
                .WillOnce(Return(-1));
        EXPECT_CALL(*mock_linux, close(_))
                .WillOnce(Return(0));

        EXPECT_CALL(*mock_linux, poll(_,1,10000))
                .WillOnce(DoAll(SetREvent(), Return(1)));
                
        EXPECT_CALL(*mock_linux, accept(1,_,_))
                .WillOnce(Return(2));
                
        // Act
        int client;
        {
                std::shared_ptr<rpp::ILinux> linux = std::move(mock_linux);
                ServerSocket socket(linux, address_);
                client = socket.accept(10.0);
        }
        
        // Assert
        ASSERT_EQ(client, 2);
}

TEST_F(serversocket_tests, accept_returns_invalid_socket_after_timeout)
{
        // Arrange
        std::shared_ptr<MockLinux> mock_linux = std::make_shared<MockLinux>();
        EXPECT_CALL(address_, get_sockaddr())
                .WillOnce(Return(sockaddr_));
        EXPECT_CALL(*mock_linux, socket(_,_,_))
                .WillOnce(Return(1));
        EXPECT_CALL(*mock_linux, bind(_,_,_))
                .WillOnce(Return(0));
        EXPECT_CALL(*mock_linux, listen(_,_))
                .WillOnce(Return(0));
        
        EXPECT_CALL(*mock_linux, shutdown(_,_))
                .WillOnce(Return(0));
        EXPECT_CALL(*mock_linux, recv(_,_,_,_))
                .WillOnce(Return(-1));
        EXPECT_CALL(*mock_linux, close(_))
                .WillOnce(Return(0));

        EXPECT_CALL(*mock_linux, poll(_,1,10000))
                .WillOnce(Return(0));
                
        // Act
        int client;
        {
                std::shared_ptr<rpp::ILinux> linux = std::move(mock_linux);
                ServerSocket socket(linux, address_);
                client = socket.accept(10.0);
        }
        
        // Assert
        ASSERT_EQ(client, kInvalidSocket);
}

TEST_F(serversocket_tests, address_returns_address)
{
    // Arrange
    rcom::Address actual_address;
    rcom::Address expected_address("10.1.1.10", 24);
    auto expected_sockaddress = expected_address.get_sockaddr();
    const struct sockaddr *pexpected_sockaddress = (struct sockaddr *)&expected_sockaddress;

    std::shared_ptr<MockLinux> mock_linux = std::make_shared<MockLinux>();
    EXPECT_CALL(*mock_linux, socket(_,_,_))
            .WillOnce(Return(1));
    EXPECT_CALL(*mock_linux, bind(_,_,_))
            .WillOnce(Return(0));
    EXPECT_CALL(*mock_linux, listen(_,_))
            .WillOnce(Return(0));
    EXPECT_CALL(*mock_linux, shutdown(_,_))
            .WillOnce(Return(0));
    EXPECT_CALL(*mock_linux, recv(_,_,_,_))
            .WillOnce(Return(0));
    EXPECT_CALL(*mock_linux, close(_))
            .WillOnce(Return(0));

    EXPECT_CALL(*mock_linux, getsockname(_,_,_))
            .WillOnce(DoAll(testing::SetArgPointee<1>(testing::ByRef(*pexpected_sockaddress)), Return(0)));

    std::shared_ptr<rpp::ILinux> linux = std::move(mock_linux);

    ServerSocket socket(linux, expected_address);

    // Act
    socket.get_address(actual_address);

    // Assert
    ASSERT_EQ(actual_address, expected_address);
}