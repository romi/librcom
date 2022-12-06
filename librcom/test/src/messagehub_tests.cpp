#include <string>
#include "gtest/gtest.h"

#include "rcom/Address.h"
#include "rcom/MessageHub.h"
#include "rcom/MessageHub.h"

#include "WebSocketServer.mock.h"
#include "SocketFactory.mock.h"
#include "MessageListener.mock.h"
#include "WebSocket.mock.h"
#include "Linux.mock.h"
#include "Log.mock.h"

using namespace rcom;
using namespace testing;

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Weffc++"

class rawmessagehub_tests : public ::testing::Test
{
protected:
	rawmessagehub_tests()
                : mockSocketFactory_(),
                  mock_linux_(),
                  mock_log_(),
                  captured_broadcast_buffer_(),
                  capture_broadcast_type_(),
                  exclude_(nullptr),
                  captured_send_buffer_() {
                mockSocketFactory_ = std::make_shared<MockSocketFactory>();
                mock_linux_ = std::make_shared<MockLinux>();
                mock_log_ = std::make_shared<MockLog>();
        }

	~rawmessagehub_tests() override = default;

	void SetUp() override {
        }
        
	void TearDown() override {
	}
        
        std::shared_ptr<MockSocketFactory> mockSocketFactory_;
        std::shared_ptr<MockLinux> mock_linux_;
        std::shared_ptr<MockLog> mock_log_;

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
                               IWebSocket *exclude) {
                captured_broadcast_buffer_ = message;
                capture_broadcast_type_ = type;
                exclude_ = exclude;
        }

        void capture_send(rcom::MemBuffer &message,
                          MessageType type) {
                captured_send_buffer_ = message;
                capture_send_type_ = type;
        }
};

#pragma GCC diagnostic pop

TEST_F(rawmessagehub_tests, construct_invalid_topic_throws)
{
        // Arrange
        std::string topic(" ");
        std::unique_ptr<IWebSocketServer> mockWsServer
                = std::make_unique<MockWebSocketServer>();                

        EXPECT_CALL(*mock_log_, error(_))
                .Times(AtLeast(1));
        // Act
        // Assert
        ASSERT_THROW(MessageHub messageHub(topic, mockWsServer,
                                           mockSocketFactory_,
                                           mock_linux_, mock_log_),
                     std::invalid_argument);
}

TEST_F(rawmessagehub_tests, construct_null_serversocket_throws)
{
        // Arrange
        std::string topic("topic");
        std::unique_ptr<IWebSocketServer> mockWsServer;                

        EXPECT_CALL(*mock_log_, error(_))
                .Times(AtLeast(1));

        // Act
        // Assert
        ASSERT_THROW(MessageHub messageHub(topic, mockWsServer,
                                           mockSocketFactory_,
                                           mock_linux_, mock_log_),
                     std::invalid_argument);
}

TEST_F(rawmessagehub_tests, construct_null_socketfactory_throws)
{
        // Arrange
        std::string topic("topic");
        std::unique_ptr<IWebSocketServer> mockWsServer
                = std::make_unique<MockWebSocketServer>();                
        const std::shared_ptr<MockSocketFactory> empty_socket_factory;
        
        EXPECT_CALL(*mock_log_, error(_))
                .Times(AtLeast(1));
        
        // Act
        // Assert
        ASSERT_THROW(MessageHub messageHub(topic, mockWsServer,
                                           empty_socket_factory,
                                           mock_linux_, mock_log_),
                     std::invalid_argument);
}

TEST_F(rawmessagehub_tests, topic_returns_topic)
{
        // Arrange
        std::string topic("topic");
        std::unique_ptr<IWebSocketServer> mockWsServer
                = std::make_unique<MockWebSocketServer>();                
        auto messageHub = std::make_unique<MessageHub>(topic,
                                                       mockWsServer,
                                                       mockSocketFactory_,
                                                       mock_linux_, mock_log_);

        // Act
        auto actual = messageHub->topic();

        // Assert
        ASSERT_EQ(actual, topic);
}

TEST_F(rawmessagehub_tests, handle_events_calls_handle_events)
{
        // Arrange
        std::string topic("topic");

        std::unique_ptr<MockWebSocketServer> mockWsServer
                = std::make_unique<MockWebSocketServer>();                
        // Set expectations before making messagehub as the unique_ptr is moved to the hub.
        EXPECT_CALL(*mockWsServer, handle_events())
                .Times(1); 
        std::unique_ptr<IWebSocketServer> server = std::move(mockWsServer);
        
        auto messageHub = std::make_unique<MessageHub>(topic, server,
                                                       mockSocketFactory_,
                                                       mock_linux_, mock_log_);

        // Act
        
        // Assert
        messageHub->handle_events();
}

TEST_F(rawmessagehub_tests, broadcast_calls_broadcast)
{
        // Arrange
        std::string topic("topic");
        rcom::MemBuffer buffer;
        buffer.append("buffer");
        
        MessageType type(MessageType::kTextMessage);
        IWebSocket* exclude = nullptr;

        std::unique_ptr<MockWebSocketServer> mockWsServer
                = std::make_unique<MockWebSocketServer>();                
        
        // Set expectations before making messagehub as the unique_ptr is moved to the hub.
        EXPECT_CALL(*mockWsServer, broadcast(_, _ , _))
                .WillOnce(testing::Invoke(this, &rawmessagehub_tests::capture_broadcast));
        std::unique_ptr<IWebSocketServer> server = std::move(mockWsServer);
        
        auto messageHub = std::make_unique<MessageHub>(topic, server,
                                                       mockSocketFactory_,
                                                       mock_linux_, mock_log_);

        // Act
        messageHub->broadcast(buffer, type, exclude);

        // Assert
        ASSERT_EQ(buffer.data(), captured_broadcast_buffer_.data());
        ASSERT_EQ(type, capture_broadcast_type_);
        ASSERT_EQ(exclude, exclude_);
}

TEST_F(rawmessagehub_tests, register_topic_registers)
{
        // Arrange
        std::string topic("topic");

        std::string address_str("100.100.100.100");
        uint16_t address_port(100);
        rcom::Address server_address(address_str.c_str(), address_port);

        rcom::MemBuffer response;
        response.printf("{\"success\":true}");
        RecvStatus recvStatus(RecvStatus::kRecvText);

        std::unique_ptr<MockWebSocketServer> mockWsServer
                = std::make_unique<MockWebSocketServer>();
        std::unique_ptr<MockWebSocket> mockClientSideWebSocket
                = std::make_unique<MockWebSocket>();

        EXPECT_CALL(*mockClientSideWebSocket, send(_, _))
                .WillOnce(Return(true));
        EXPECT_CALL(*mockClientSideWebSocket, close( _));

        EXPECT_CALL(*mockClientSideWebSocket, recv(_, _))
                .WillOnce(DoAll(SetArgReferee<0>(response), Return(recvStatus)));

        EXPECT_CALL(*mockWsServer, get_address(_))
                .WillOnce(SetArgReferee<0>(server_address));

        EXPECT_CALL(*mockSocketFactory_, new_client_side_websocket(_))
                .WillOnce(Return(ByMove(std::move(mockClientSideWebSocket))));

        std::unique_ptr<IWebSocketServer> server = std::move(mockWsServer);

        auto messageHub = std::make_unique<MessageHub>(topic, server,
                                                       mockSocketFactory_,
                                                       mock_linux_, mock_log_);
        
        // Act
        messageHub->register_topic();
        
        // Assert
}
