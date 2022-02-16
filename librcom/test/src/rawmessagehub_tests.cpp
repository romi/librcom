#include <string>
#include <WebSocketServer.mock.h>
#include <SocketFactory.mock.h>
#include <Address.h>
#include "gtest/gtest.h"

#include "RawMessageHub.h"
#include "MessageHub.h"
#include "MessageListener.mock.h"
#include "WebSocketServerFactory.mock.h"
#include "WebSocket.mock.h"

using namespace rcom;
using namespace testing;

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Weffc++"

class rawmessagehub_tests : public ::testing::Test
{
protected:
	rawmessagehub_tests()
    : mockMessageListener_(), mockSocketFactory_(),
      mockWebSocketServerFactory_(), mockWebSocketServer_(),
      captured_broadcast_buffer_(), capture_broadcast_type_(), exclude_(nullptr),
      captured_send_buffer_()
    {
    }

	~rawmessagehub_tests() override = default;

	void SetUp() override {
            mockMessageListener_ = std::make_shared<MockMessageListener>();
            mockSocketFactory_ = std::make_shared<MockSocketFactory>();
            mockWebSocketServerFactory_ = std::make_shared<MockWebSocketServerFactory>();
            mockWebSocketServer_ = std::make_unique<MockWebSocketServer>();
        }
        
	void TearDown() override {
	}

    std::shared_ptr<RawMessageHub> MakeRawMessageHub(std::string& topic)
    {
        EXPECT_CALL(*mockWebSocketServerFactory_, new_web_socket_server(_,_))
                .WillOnce(Return(ByMove(std::move(mockWebSocketServer_))));

        return std::make_shared<RawMessageHub>(topic, mockMessageListener_, mockSocketFactory_, mockWebSocketServerFactory_);
    }

    std::shared_ptr<MockMessageListener> mockMessageListener_;
    std::shared_ptr<MockSocketFactory> mockSocketFactory_;
    std::shared_ptr<MockWebSocketServerFactory> mockWebSocketServerFactory_;
    std::unique_ptr<MockWebSocketServer> mockWebSocketServer_;

    // Broacast capture
    rcom::MemBuffer captured_broadcast_buffer_;
    MessageType capture_broadcast_type_;
    IWebSocket *exclude_;

    // Send capture
    rcom::MemBuffer captured_send_buffer_;
    MessageType capture_send_type_;

public:
    void capture_broadcast(rcom::MemBuffer &message,
                        MessageType type,
                        IWebSocket *exclude)
    {
        captured_broadcast_buffer_ = message;
        capture_broadcast_type_ = type;
        exclude_ = exclude;
    }

    void capture_send(rcom::MemBuffer &message,
                           MessageType type)
    {
        captured_send_buffer_ = message;
        capture_send_type_ = type;
    }
};

#pragma GCC diagnostic pop

TEST_F(rawmessagehub_tests, construct_invalid_topic_throws)
{
        // Arrange
        auto mockMessageListener = std::make_shared<MockMessageListener>();
        auto mockSocketFactory = std::make_shared<MockSocketFactory>();
        auto mockWebServerSocketFactory = std::make_shared<MockWebSocketServerFactory>();
        std::string topic(" ");

        // Act
        //Assert
        ASSERT_THROW(RawMessageHub rawMessageHub(topic, mockMessageListener, mockSocketFactory, mockWebServerSocketFactory), std::invalid_argument);
}

TEST_F(rawmessagehub_tests, construct_null_websocketserverfatory_throws)
{
    // Arrange
    auto mockMessageListener = std::make_shared<MockMessageListener>();
    auto mockSocketFactory = std::make_shared<MockSocketFactory>();
    std::shared_ptr< MockWebSocketServerFactory> mockWebSocketServerFactory = nullptr;
    std::string topic("topic");

    // Act
    //Assert
    ASSERT_THROW(RawMessageHub rawMessageHub(topic, mockMessageListener, mockSocketFactory, mockWebSocketServerFactory), std::invalid_argument);
}

TEST_F(rawmessagehub_tests, construct_null_socketfactory_throws)
{
    // Arrange
    auto mockMessageListener = std::make_shared<MockMessageListener>();
    std::shared_ptr<MockSocketFactory> mockSocketFactory = nullptr;
    std::shared_ptr< MockWebSocketServerFactory> mockWebSocketServerFactory = std::make_shared<MockWebSocketServerFactory>();
    std::string topic("topic");

    auto mockWebSocketServer = std::make_unique<MockWebSocketServer>();

    // Act
    //Assert
    ASSERT_THROW(RawMessageHub rawMessageHub(topic, mockMessageListener, mockSocketFactory, mockWebSocketServerFactory), std::invalid_argument);
}

TEST_F(rawmessagehub_tests, construct_null_websocketserver_returned_throws)
{
    // Arrange
    auto mockMessageListener = std::make_shared<MockMessageListener>();
    auto mockSocketFactory = std::make_shared<MockSocketFactory>();
    std::shared_ptr< MockWebSocketServerFactory> mockWebSocketServerFactory = std::make_shared<MockWebSocketServerFactory>();
    std::string topic("topic");

    std::unique_ptr<MockWebSocketServer> mockWebSocketServer = nullptr;

    EXPECT_CALL(*mockWebSocketServerFactory, new_web_socket_server(_,_))
            .WillOnce(Return(ByMove(std::move(mockWebSocketServer))));

    // Act
    //Assert
    ASSERT_THROW(RawMessageHub rawMessageHub(topic, mockMessageListener, mockSocketFactory, mockWebSocketServerFactory), std::invalid_argument);
}

TEST_F(rawmessagehub_tests, construct_success_does_not_throw)
{
    // Arrange
    auto mockMessageListener = std::make_shared<MockMessageListener>();
    auto mockSocketFactory = std::make_shared<MockSocketFactory>();
    std::shared_ptr< MockWebSocketServerFactory> mockWebSocketServerFactory = std::make_shared<MockWebSocketServerFactory>();
    std::string topic("topic");

    auto mockWebSocketServer = std::make_unique<MockWebSocketServer>();

    EXPECT_CALL(*mockWebSocketServerFactory, new_web_socket_server(_,_))
            .WillOnce(Return(ByMove(std::move(mockWebSocketServer))));

    // Act
    //Assert
    ASSERT_NO_THROW(RawMessageHub rawMessageHub(topic, mockMessageListener, mockSocketFactory, mockWebSocketServerFactory));
}

TEST_F(rawmessagehub_tests, topic_returns_topic)
{
    // Arrange
    std::string topic("topic");
    auto rawMessageHub = MakeRawMessageHub(topic);

    // Act
    auto actual = rawMessageHub->topic();

    //Assert
    ASSERT_EQ(actual, topic);
}

TEST_F(rawmessagehub_tests, handle_events_calls_handle_events)
{
    // Arrange
    std::string topic("topic");

    // Set expectations before making messagehub as the unique_ptr is moved to the hub.
    EXPECT_CALL(*mockWebSocketServer_, handle_events())
            .Times(1);
    auto rawMessageHub = MakeRawMessageHub(topic);

    // Act
    //Assert
    rawMessageHub->handle_events();
}

TEST_F(rawmessagehub_tests, broadcast_calls_broadcast)
{
    // Arrange
    std::string topic("topic");
    rcom::MemBuffer buffer;
    buffer.append_string("buffer");
    MessageType type(MessageType::kTextMessage);
    IWebSocket* exclude = nullptr;
    // Set expectations before making messagehub as the unique_ptr is moved to the hub.
    EXPECT_CALL(*mockWebSocketServer_, broadcast(_ ,_ , _))
            .WillOnce(testing::Invoke(this, &rawmessagehub_tests::capture_broadcast));
    auto rawMessageHub = MakeRawMessageHub(topic);

    // Act
    rawMessageHub->broadcast(buffer, type, exclude);

    //Assert
    ASSERT_EQ(buffer.data(), captured_broadcast_buffer_.data());
    ASSERT_EQ(type, capture_broadcast_type_);
    ASSERT_EQ(exclude, exclude_);
}

TEST_F(rawmessagehub_tests, count_links_returns_correct_value)
{
    // Arrange
    size_t expected = 5;
    std::string topic("topic");
    // Set expectations before making messagehub as the unique_ptr is moved to the hub.
    EXPECT_CALL(*mockWebSocketServer_, count_links())
            .WillOnce(Return(expected));
    auto rawMessageHub = MakeRawMessageHub(topic);

    // Act
    auto actual = rawMessageHub->count_links();

    //Assert
    ASSERT_EQ(actual, expected);
}

TEST_F(rawmessagehub_tests, register_topic_registers)
{
    // Arrange
    auto mockMessageListener = std::make_shared<MockMessageListener>();
    auto mockSocketFactory = std::make_shared<MockSocketFactory>();
    std::shared_ptr< MockWebSocketServerFactory> mockWebSocketServerFactory = std::make_shared<MockWebSocketServerFactory>();
    std::string topic("topic");

    std::string address_str("100.100.100.100");
    uint16_t address_port(100);
    Address server_address(address_str.c_str(), address_port);

    rcom::MemBuffer response;
    response.printf("{\"success\":true}");
    RecvStatus recvStatus(RecvStatus::kRecvText);


    auto mockWebSocketServer = std::make_unique<MockWebSocketServer>();
    auto mockClientSideWebSocket = std::make_unique<MockWebSocket>();

    EXPECT_CALL(*mockClientSideWebSocket, send(_, _))
            .WillOnce(Return(true));
    EXPECT_CALL(*mockClientSideWebSocket, close( _));

    EXPECT_CALL(*mockClientSideWebSocket, recv(_, _))
            .WillOnce(DoAll(SetArgReferee<0>(response), Return(recvStatus)));

    EXPECT_CALL(*mockWebSocketServer, get_address(_))
            .WillOnce(SetArgReferee<0>(server_address));

    EXPECT_CALL(*mockWebSocketServerFactory, new_web_socket_server(_,_))
            .WillOnce(Return(ByMove(std::move(mockWebSocketServer))));

    EXPECT_CALL(*mockSocketFactory, new_client_side_websocket(_))
            .WillOnce(Return(ByMove(std::move(mockClientSideWebSocket))));

        // Act
        //Assert
        MessageHub rawMessageHub(topic, mockMessageListener, mockSocketFactory, mockWebSocketServerFactory);


}