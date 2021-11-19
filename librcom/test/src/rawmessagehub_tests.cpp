#include <string>
#include <WebSocketServer.mock.h>
#include "gtest/gtest.h"

// Need to include r.h here to make sure when we mock them below they
// are mocked in the link with the tests.
#include "RawMessageHub.h"
#include "MessageListener.mock.h"
#include "WebSocketServerFactory.mock.h"

extern "C" {
#include "log.mock.h"
}

using namespace rcom;
using namespace testing;

class rawmessagehub_tests : public ::testing::Test
{
protected:
	rawmessagehub_tests() = default;

	~rawmessagehub_tests() override = default;

	void SetUp() override {
        RESET_FAKE(r_err);
        RESET_FAKE(r_warn);
        RESET_FAKE(r_panic);
        }
        
	void TearDown() override {
	}
};

TEST_F(rawmessagehub_tests, construct_invalid_topic_throws)
{
        // Arrange
        auto mockMessageListener = std::make_shared<MockMessageListener>();
        auto mockWebServerSocketFactory = std::make_shared<MockWebSocketServerFactory>();
        std::string topic(" ");

        // Act
        //Assert
        ASSERT_THROW(RawMessageHub rawMessageHub(topic, mockMessageListener, mockWebServerSocketFactory), std::invalid_argument);
        ASSERT_EQ(r_err_fake.call_count, 1);
}

TEST_F(rawmessagehub_tests, construct_null_websocketserverfatory_throws)
{
    // Arrange
    auto mockMessageListener = std::make_shared<MockMessageListener>();
    std::shared_ptr< MockWebSocketServerFactory> mockWebSocketServerFactory = nullptr;
    std::string topic("topic");

    // Act
    //Assert
    ASSERT_THROW(RawMessageHub rawMessageHub(topic, mockMessageListener, mockWebSocketServerFactory), std::invalid_argument);
    ASSERT_EQ(r_err_fake.call_count, 1);
}


TEST_F(rawmessagehub_tests, construct_null_websocketserver_returned_throws)
{
    // Arrange
    auto mockMessageListener = std::make_shared<MockMessageListener>();
    std::shared_ptr< MockWebSocketServerFactory> mockWebSocketServerFactory = std::make_shared<MockWebSocketServerFactory>();
    std::string topic("topic");

    std::unique_ptr<MockWebSocketServer> mockWebSocketServer = nullptr;
//    auto webSocketServer = std::make_unique<MockWebSocketServer>();
//
    EXPECT_CALL(*mockWebSocketServerFactory, new_web_socket_server(_,_))
            .WillOnce(Return(ByMove(std::move(mockWebSocketServer))));

    // Act
    //Assert
    ASSERT_THROW(RawMessageHub rawMessageHub(topic, mockMessageListener, mockWebSocketServerFactory), std::invalid_argument);
    ASSERT_EQ(r_err_fake.call_count, 1);
}

TEST_F(rawmessagehub_tests, construct_success_does_not_throw)
{
    // Arrange
    auto mockMessageListener = std::make_shared<MockMessageListener>();
    std::shared_ptr< MockWebSocketServerFactory> mockWebSocketServerFactory = std::make_shared<MockWebSocketServerFactory>();
    std::string topic("topic");

    auto mockWebSocketServer = std::make_unique<MockWebSocketServer>();

    EXPECT_CALL(*mockWebSocketServerFactory, new_web_socket_server(_,_))
            .WillOnce(Return(ByMove(std::move(mockWebSocketServer))));

    // Act
    //Assert
    ASSERT_NO_THROW(RawMessageHub rawMessageHub(topic, mockMessageListener, mockWebSocketServerFactory));
    ASSERT_EQ(r_err_fake.call_count, 0);
}