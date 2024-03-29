#include "gtest/gtest.h"
#include "rcom/WebSocketServer.h"
#include "rcom/Log.h"

#include "ServerSocket.mock.h"
#include "SocketFactory.mock.h"
#include "MessageListener.mock.h"
#include "WebSocket.mock.h"
#include "Log.mock.h"

using namespace std;
using namespace rcom;

using ::testing::Return;
using ::testing::ByMove;
using ::testing::_;
using ::testing::AtLeast;

class websocketserver_tests : public ::testing::Test
{
public:
        
protected:

        std::shared_ptr<MockSocketFactory> mock_factory_;
        std::shared_ptr<rcom::ISocketFactory> factory_;
        std::shared_ptr<MockMessageListener> mock_listener_;
        std::shared_ptr<MockLog> mock_log_;
        std::shared_ptr<rcom::IMessageListener> listener_;
        rcom::MemBuffer read_data_;
        size_t current_read_char_;


        // CLOSE_GOING_AWAY
        // FIN(0x80)+CLOSE(0x08) + MASK(0x80)|LEN(2) + MASK(4) + CODE(1001=0x03e9)
        static constexpr const uint8_t
        client_1001_close_handshake[] = { 0x88, 0x82,
                                          0x00, 0x00, 0x00, 0x00,
                                          0x03, 0xe9 };
        
        websocketserver_tests()
                : mock_factory_(),
                  factory_(),
                  mock_log_(),
                  read_data_(),
                  current_read_char_(0) {
                mock_factory_ = std::make_shared<MockSocketFactory>();
                factory_ = mock_factory_;
                mock_listener_ = std::make_shared<MockMessageListener>();
                mock_log_ = std::make_shared<MockLog>();
                listener_ = mock_listener_;
        }
        
        ~websocketserver_tests() override = default;

        void SetUp() override {
                read_data_.clear();
        }

        void TearDown() override {
        }

        void set_read_data(const char *data) {
                read_data_.clear();
                read_data_.append_string_32k_max(data);
        }

        void set_read_data(const uint8_t *data, size_t length) {
                read_data_.clear();
                read_data_.append(data, length);
        }

public:
        RecvStatus copy_input(rcom::MemBuffer& message, double timeout) {
                (void) timeout;
                message.append(read_data_);
                return kRecvText;
        }

        void debug_close(CloseCode code) {
                log_warn(mock_log_, "debug_close %d", code);
        }
};

TEST_F(websocketserver_tests, new_websocketserver_is_successful)
{
        // Arrange
        MockServerSocket *server_socket = new MockServerSocket();
        EXPECT_CALL(*server_socket, close());
        unique_ptr<IServerSocket> i_server_socket(server_socket);

        // Act
        unique_ptr<WebSocketServer> server
                = make_unique<WebSocketServer>(i_server_socket, factory_,
                                               listener_, mock_log_);

        // Assert
}

TEST_F(websocketserver_tests, accepts_new_connections_and_calls_onconnect)
{
        // Arrange
        MockServerSocket *server_socket = new MockServerSocket();
        EXPECT_CALL(*server_socket, close());
        EXPECT_CALL(*server_socket, accept(_))
                .WillOnce(Return(3))
                .WillRepeatedly(Return(-1));
        unique_ptr<IServerSocket> i_server_socket(server_socket);

        MockWebSocket *websocket = new MockWebSocket();
        EXPECT_CALL(*websocket, is_connected())
                .WillRepeatedly(Return(true));
        EXPECT_CALL(*websocket, close(_));
        EXPECT_CALL(*websocket, recv(_,_))
                .WillRepeatedly(Return(kRecvTimeOut));
        set_read_data(client_1001_close_handshake, sizeof(client_1001_close_handshake)); 

        EXPECT_CALL(*mock_factory_, new_server_side_websocket(_))
                .WillOnce(Return(ByMove(unique_ptr<IWebSocket>(websocket))));

        // Act
        unique_ptr<WebSocketServer> server
                = make_unique<WebSocketServer>(i_server_socket, factory_,
                                               listener_, mock_log_);
        server->handle_events();
        
        // Assert
        ASSERT_EQ(server->count_links(), 1);
}

TEST_F(websocketserver_tests, close_message_removes_link)
{
        // Arrange
        MockServerSocket *server_socket = new MockServerSocket();
        EXPECT_CALL(*server_socket, close());
        EXPECT_CALL(*server_socket, accept(_))
                .WillOnce(Return(3))
                .WillRepeatedly(Return(-1));
        unique_ptr<IServerSocket> i_server_socket(server_socket);

        MockWebSocket *websocket = new MockWebSocket();
        EXPECT_CALL(*websocket, is_connected())
                .WillOnce(Return(true))
                .WillOnce(Return(false));
        EXPECT_CALL(*websocket, recv(_,_))
                .WillOnce(Return(kRecvClosed));

        EXPECT_CALL(*mock_factory_, new_server_side_websocket(_))
                .WillOnce(Return(ByMove(unique_ptr<IWebSocket>(websocket))));

        // Act
        unique_ptr<WebSocketServer> server
                = make_unique<WebSocketServer>(i_server_socket, factory_,
                                               listener_, mock_log_);
        server->handle_events();
        
        // Assert
        ASSERT_EQ(server->count_links(), 0);
}

TEST_F(websocketserver_tests, recv_error_closes_and_removes_link)
{
        // Arrange
        MockServerSocket *server_socket = new MockServerSocket();
        EXPECT_CALL(*server_socket, close());
        EXPECT_CALL(*server_socket, accept(_))
                .WillOnce(Return(3))
                .WillRepeatedly(Return(-1));
        unique_ptr<IServerSocket> i_server_socket(server_socket);

        MockWebSocket *websocket = new MockWebSocket();
        EXPECT_CALL(*websocket, is_connected())
                .WillOnce(Return(true))
                .WillOnce(Return(false));
        EXPECT_CALL(*websocket, recv(_,_))
                .WillOnce(Return(kRecvError));
        EXPECT_CALL(*websocket, close(_));

        EXPECT_CALL(*mock_factory_, new_server_side_websocket(_))
                .WillOnce(Return(ByMove(unique_ptr<IWebSocket>(websocket))));

        EXPECT_CALL(*mock_log_, error(_))
                .Times(AtLeast(1));
        
        // Act
        unique_ptr<WebSocketServer> server
                = make_unique<WebSocketServer>(i_server_socket, factory_,
                                               listener_, mock_log_);
        server->handle_events();
        
        // Assert
        ASSERT_EQ(server->count_links(), 0);
}

ACTION(ThrowRuntimeException)
{
        throw std::runtime_error("");
}

TEST_F(websocketserver_tests, webserver_logs_error_when_factory_throws_exception)
{
        // Arrange
        MockServerSocket *server_socket = new MockServerSocket();
        EXPECT_CALL(*server_socket, close());
        EXPECT_CALL(*server_socket, accept(_))
                .WillOnce(Return(3))
                .WillRepeatedly(Return(-1));
        unique_ptr<IServerSocket> i_server_socket(server_socket);

        EXPECT_CALL(*mock_factory_, new_server_side_websocket(_))
                .WillOnce(ThrowRuntimeException());        

        EXPECT_CALL(*mock_log_, error(_))
                .Times(AtLeast(1));

        // Act
        unique_ptr<WebSocketServer> server
                = make_unique<WebSocketServer>(i_server_socket, factory_,
                                               listener_, mock_log_);
        server->handle_events();
        
        // Assert
        ASSERT_EQ(server->count_links(), 0);
}

TEST_F(websocketserver_tests, failed_send_removes_link)
{
        // Arrange
        MockServerSocket *server_socket = new MockServerSocket();
        EXPECT_CALL(*server_socket, close());
        EXPECT_CALL(*server_socket, accept(_))
                .WillOnce(Return(3))
                .WillRepeatedly(Return(-1));
        unique_ptr<IServerSocket> i_server_socket(server_socket);

        MockWebSocket *websocket = new MockWebSocket();
        EXPECT_CALL(*websocket, is_connected())
                .WillRepeatedly(Return(true));
        EXPECT_CALL(*websocket, recv(_,_))
                .WillOnce(Return(kRecvTimeOut));
        EXPECT_CALL(*websocket, send(_,_))
                .WillOnce(Return(false));
        EXPECT_CALL(*websocket, close(_))
                .WillRepeatedly(Invoke(this, &websocketserver_tests::debug_close));

        EXPECT_CALL(*mock_factory_, new_server_side_websocket(_))
                .WillOnce(Return(ByMove(unique_ptr<IWebSocket>(websocket))));

        EXPECT_CALL(*mock_log_, warn(_))
                .Times(AtLeast(1));
        
        rcom::MemBuffer message;
        message.append("abc");
        
        // Act
        unique_ptr<WebSocketServer> server
                = make_unique<WebSocketServer>(i_server_socket, factory_,
                                               listener_, mock_log_);
        server->handle_events();
        server->broadcast(message, kTextMessage, nullptr);
        
        // Assert
}
