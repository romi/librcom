#include <iostream>
#include "gtest/gtest.h"

#include "rcom/ClientSideWebSocket.h"
#include "rcom/ServerSideWebSocket.h"
#include "rcom/Address.h"

#include "Socket.mock.h"
#include "RequestParser.mock.h"
#include "Request.mock.h"
#include "ResponseParser.mock.h"
#include "Response.mock.h"
#include "Linux.mock.h"
#include "Log.mock.h"

using namespace std;
using namespace rcom;

using ::testing::Return;
using ::testing::ReturnRef;
using ::testing::_;
using ::testing::SetArgReferee;
using ::testing::NiceMock;
using ::testing::Assign;
using ::testing::ReturnPointee;
using ::testing::AtLeast;

ACTION(ThrowRuntimeException)
{
        throw std::runtime_error("mocked runtime exception");
}

class clientside_websocket_tests : public ::testing::Test
{
public:
        MockRequestParser mock_request_parser_;
        MockRequest mock_request_;
        MockResponseParser mock_response_parser_;
        MockResponse mock_response_;
        std::shared_ptr<MockLinux> mock_linux_;
        std::shared_ptr<MockLog> mock_log_;
        MemBuffer input_data_;
        size_t input_data_offset_;
        vector<MemBuffer> output_data_;
        string request_key_header_value_;
        string response_accept_header_value_;
        uint8_t fill_buffer_[1024];
        bool socket_connected_;
        
        // CLOSE_GOING_AWAY
        // FIN(0x80)+CLOSE(0x08) + LEN(2) + CODE(1001=0x03e9)
        static constexpr const uint8_t server_1001_close_handshake[] = { 0x88, 0x02, 0x03, 0xe9 };

        // CLOSE_PROTOCOL_ERROR
        // FIN(0x80)+CLOSE(0x08) + LEN(2) + CODE(1002=0x03ea)
        static constexpr const uint8_t server_1002_close_handshake[] = { 0x88, 0x02, 0x03, 0xea };

        // CLOSE_TOO_BIG
        // FIN(0x80)+CLOSE(0x08) + LEN(2) + CODE(1009=0x03f1)
        static constexpr const uint8_t server_1009_close_handshake[] = { 0x88, 0x02, 0x03, 0xf1 };
        
        // FIN(0x80)|CLOSE(0x08) + LEN(2) + CODE(1001=0x03e9)
        static constexpr const uint8_t server_close_reply[] = { 0x88, 0x02, 0x03, 0xe9 };

        // CLOSE_GOING_AWAY
        // FIN(0x80)+CLOSE(0x08) + MASK(0x80)|LEN(2) + MASK(4) + CODE(1001=0x03e9)
        static constexpr const uint8_t client_1001_close_handshake[] = { 0x88, 0x82,
                                                                         0x00, 0x00, 0x00, 0x00,
                                                                         0x03, 0xe9 };
        
        // FIN(0x80)|CLOSE(0x08) + MASK(0x80)|LEN(2) + MASK(4) + CODE(1001=0x03e9)
        static constexpr const uint8_t client_close_reply[] = { 0x88, 0x82,
                                                                0x00, 0x00, 0x00, 0x00,
                                                                0x03, 0xe9 };
                
        // FIN(0x80)|TEXT(0x01) + MASK(0x80)|LEN(3) + MASK(4) + DATA('abc')
        static constexpr const uint8_t masked_text_message[] = { 0x81, 0x83,
                                                                 0x00, 0x00, 0x00, 0x00,
                                                                 'a', 'b', 'c' };
        
        // FIN(0x80)|BINARY(0x02) + MASK(0x80)|LEN(3) + MASK(4) + DATA('abc')
        static constexpr const uint8_t masked_binary_message[] = { 0x82, 0x83,
                                                            0x00, 0x00, 0x00, 0x00,
                                                            'a', 'b', 'c' };
                
        // FIN(0x80)|TEXT(0x01) + LEN(3) + DATA('abc')
        static constexpr const uint8_t unmasked_text_message[] = { 0x81, 0x03, 'a', 'b', 'c' };
                
        // FIN(0x00)|TEXT(0x01) + MASK(0x80)|LEN(1) + MASK(4) + DATA('a')
        static constexpr const uint8_t fragmented_message_a[] = { 0x01, 0x81,
                                                                  0x00, 0x00, 0x00, 0x00,
                                                                  'a' };
        
        // FIN(0x00)|CONTINUATION(0x00) + MASK(0x80)|LEN(1) + MASK(4) + DATA('b')
        static constexpr const uint8_t fragmented_message_b[] = { 0x00, 0x81,
                                                                  0x00, 0x00, 0x00, 0x00,
                                                                  'b' }; 
        
        // FIN(0x80)|CONTINUATION(0x00) + MASK(0x80)|LEN(1) + MASK(4) + DATA('c')
        static constexpr const uint8_t fragmented_message_c[] = { 0x80, 0x81,
                                                                  0x00, 0x00, 0x00, 0x00,
                                                                  'c' };
        
        // FIN(0x80)|BINARY(0x02) + MASK(0x80)|LEN(126=0x7e) + LEN(126=0x007e) + MASK(4)
        static constexpr const uint8_t binary_frame_len126[] = { 0x82, 0xfe, 0x00, 0x7e,
                                                                 0x00, 0x00, 0x00, 0x00};
        
        // FIN(0x80)|BINARY(0x02) + MASK(0x80)|LEN(127=0x7f) + LEN(65536=0x...010000) + MASK(4)
        static constexpr const uint8_t binary_frame_len65536[] = { 0x82, 0xff, 
                                                                   0x00, 0x00, 0x00, 0x00,
                                                                   0x00, 0x01, 0x00, 0x00,
                                                                   0x00, 0x00, 0x00, 0x00};
        
        
        
protected:
        
        clientside_websocket_tests() :
                mock_request_parser_(),
                mock_request_(),
                mock_response_parser_(),
                mock_response_(),
                mock_linux_(),
                mock_log_(),
                input_data_(),
                input_data_offset_(0),
                output_data_(),
                request_key_header_value_(),
                response_accept_header_value_(),
                socket_connected_(true) {

                request_key_header_value_ = "AAAAAAAAAAAAAAAAAAAAAA==";
                response_accept_header_value_ = "ICX+Yqv66kxgM0FcWaLWlFLwTAI=";
                
                for (int i = 0; i < 64; i++)
                        memcpy(&fill_buffer_[i*16], "0123456789abcdef", 16);

                mock_linux_ = std::make_shared<MockLinux>();
                mock_log_ = std::make_shared<MockLog>();
        }

        ~clientside_websocket_tests() override {
        }

        void SetUp() override {
                output_data_.clear();
                input_data_.clear();
                socket_connected_ = true;
        }

        void TearDown() override {
        }

public:
        void input_append(const uint8_t *buffer, size_t length) {
                input_data_.append(buffer, length);
        }

        void input_append_random_data(size_t length) {
                size_t count = 0;
                while (count < length) {
                        size_t n = 1024;
                        if (count + n > length)
                                n = length - count;
                        input_append(fill_buffer_, n);
                        count += n;
                }
        }

        void input_append_client_1001_close_handshake() {
                input_append(client_1001_close_handshake,
                             sizeof(client_1001_close_handshake));
        }

        void input_append_server_1001_close_handshake() {
                input_append(server_1001_close_handshake,
                             sizeof(server_1001_close_handshake));
        }

        void copy_output(MemBuffer& socket_output) {
                output_data_.emplace_back(socket_output);
        }
        
        void copy_input(uint8_t *buffer, size_t length) {
                const vector<uint8_t>& data = input_data_.data();                
                for (size_t i = 0;
                     i < length && input_data_offset_ < data.size();
                     i++, input_data_offset_++) {
                        
                        buffer[i] = data[input_data_offset_];
                }
        }

};


using ::testing::Invoke;
ssize_t getrandom_fake(void *buf, size_t buflen, unsigned int flags)
{
    (void) flags;
    memset(buf, 0, buflen);
    return (ssize_t)buflen;
}

TEST_F(clientside_websocket_tests, successfull_creation_and_delete_of_client_side_websocket)
{
        // Arrange
        
        std::unique_ptr<MockSocket> mock_socket = make_unique<MockSocket>();
        
        EXPECT_CALL(mock_response_parser_, parse(_))
                .RetiresOnSaturation();
        
        EXPECT_CALL(mock_response_parser_, response())
                .WillRepeatedly(ReturnRef(mock_response_));
        
        EXPECT_CALL(mock_response_, assert_websocket(_));

        EXPECT_CALL(*mock_socket, send(_))
                .WillRepeatedly(DoAll(Invoke(this,
                                             &clientside_websocket_tests::copy_output),
                                      Return(true)));

        EXPECT_CALL(*mock_socket, wait(_))
                .WillRepeatedly(Return(WaitStatus::kWaitOK));

        EXPECT_CALL(*mock_socket, read(_,_))
                .WillRepeatedly(DoAll(Invoke(this, &clientside_websocket_tests::copy_input),
                                      Return(true)));

        EXPECT_CALL(*mock_socket, is_endpoint_connected())
                .WillOnce(Return(false));
        
        EXPECT_CALL(*mock_socket, close());

        EXPECT_CALL(*mock_linux_, clock_gettime(_,_))
                .WillRepeatedly(Return(0.0));


        EXPECT_CALL(*mock_linux_, getrandom(_,_,_))
                .Times(2)
                .WillOnce(Invoke(getrandom_fake))
                .WillOnce(Invoke(getrandom_fake));
        
        input_append_server_1001_close_handshake();
                                                           
        std::unique_ptr<ISocket> socket = std::move(mock_socket);
        Address remote_address;

        // Act
        std::unique_ptr<ClientSideWebSocket> websocket
                = make_unique<ClientSideWebSocket>(socket, mock_response_parser_,
                                                   remote_address,
                                                   mock_linux_, mock_log_);
        websocket->close(kCloseGoingAway);
        
        // Assert

        ASSERT_EQ(output_data_.size(), 2); 

        // HTTP handshake
        ASSERT_EQ(output_data_[0].tostring()
                  .find("GET / HTTP/1.1"), 0);
        ASSERT_NE(output_data_[0].tostring().find("Connection: Upgrade"),
                  std::string::npos);
        ASSERT_NE(output_data_[0].tostring().find("Upgrade: websocket"),
                  std::string::npos);
        ASSERT_NE(output_data_[0].tostring().find("Sec-WebSocket-Version: 13"),
                  std::string::npos);
        ASSERT_NE(output_data_[0].tostring().find("Sec-WebSocket-Key"),
                  std::string::npos);

        // Close message
        ASSERT_EQ(output_data_[1].size(), sizeof(client_1001_close_handshake));
        ASSERT_EQ(memcmp(output_data_[1].data().data(),
                         client_1001_close_handshake,
                         sizeof(client_1001_close_handshake)), 0);
}

TEST_F(clientside_websocket_tests, when_endpoint_connected_close_connection_waits_for_disconnected_endpoint)
{
        // Arrange

        std::unique_ptr<MockSocket> mock_socket = make_unique<MockSocket>();

        EXPECT_CALL(mock_response_parser_, parse(_))
                .RetiresOnSaturation();

        EXPECT_CALL(mock_response_parser_, response())
                .WillRepeatedly(ReturnRef(mock_response_));

        EXPECT_CALL(mock_response_, assert_websocket(_));

        EXPECT_CALL(*mock_socket, send(_))
                .WillRepeatedly(DoAll(Invoke(this,
                                             &clientside_websocket_tests::copy_output),
                                      Return(true)));

        EXPECT_CALL(*mock_socket, wait(_))
                .WillRepeatedly(Return(WaitStatus::kWaitOK));

        EXPECT_CALL(*mock_socket, read(_,_))
                .WillRepeatedly(DoAll(Invoke(this, &clientside_websocket_tests::copy_input),
                                      Return(true)));

        EXPECT_CALL(*mock_socket, is_endpoint_connected())
                .WillOnce(Return(true))
                .WillOnce(Return(false));

        EXPECT_CALL(*mock_socket, close());

        EXPECT_CALL(*mock_linux_, clock_gettime(_,_))
                .WillRepeatedly(Return(0.0));
        EXPECT_CALL(*mock_linux_, clock_nanosleep(_,_,_,_))
                .Times(AtLeast(1));

        EXPECT_CALL(*mock_linux_, getrandom(_,_,_))
                .Times(2)
                .WillOnce(Invoke(getrandom_fake))
                .WillOnce(Invoke(getrandom_fake));

        input_append_server_1001_close_handshake();

        std::unique_ptr<ISocket> socket = std::move(mock_socket);
        Address remote_address;

        // Act
        std::unique_ptr<ClientSideWebSocket> websocket
                = make_unique<ClientSideWebSocket>(socket, mock_response_parser_,
                                                   remote_address, mock_linux_, mock_log_);
        websocket->close(kCloseGoingAway);

        // Assert

        ASSERT_EQ(output_data_.size(), 2);

        // HTTP handshake
        ASSERT_EQ(output_data_[0].tostring()
                  .find("GET / HTTP/1.1"), 0);
        ASSERT_NE(output_data_[0].tostring().find("Connection: Upgrade"),
                  std::string::npos);
        ASSERT_NE(output_data_[0].tostring().find("Upgrade: websocket"),
                  std::string::npos);
        ASSERT_NE(output_data_[0].tostring().find("Sec-WebSocket-Version: 13"),
                  std::string::npos);
        ASSERT_NE(output_data_[0].tostring().find("Sec-WebSocket-Key"),
                  std::string::npos);

        // Close message
        ASSERT_EQ(output_data_[1].size(), sizeof(client_1001_close_handshake));
        ASSERT_EQ(memcmp(output_data_[1].data().data(),
                         client_1001_close_handshake,
                         sizeof(client_1001_close_handshake)), 0);
}

TEST_F(clientside_websocket_tests, new_client_side_websocket_throws_error_if_send_fails)
{
        // Arrange
        
        std::unique_ptr<MockSocket> mock_socket = make_unique<MockSocket>();

        EXPECT_CALL(*mock_socket, send(_))
                .WillOnce(Return(false));

        EXPECT_CALL(*mock_socket, is_endpoint_connected())
                .WillOnce(Return(false));

        EXPECT_CALL(*mock_socket, close());
        EXPECT_CALL(*mock_linux_, clock_gettime(_,_));

        EXPECT_CALL(*mock_linux_, getrandom(_,_,_))
                .WillOnce(Invoke(getrandom_fake));

        EXPECT_CALL(*mock_log_, error(_))
                .Times(AtLeast(1));
                                                           
        std::unique_ptr<ISocket> socket = std::move(mock_socket);
        Address remote_address;
        
        // Act
        ASSERT_THROW(std::unique_ptr<ClientSideWebSocket> websocket
                     = make_unique<ClientSideWebSocket>(socket, mock_response_parser_,
                                                        remote_address, mock_linux_,
                                                        mock_log_),
                     std::runtime_error);
}

TEST_F(clientside_websocket_tests, new_client_side_websocket_throws_error_on_failed_parse)
{
        // Arrange
        
        std::unique_ptr<MockSocket> mock_socket = make_unique<MockSocket>();
        
        EXPECT_CALL(mock_response_parser_, parse(_))
                .WillOnce(ThrowRuntimeException())
                .RetiresOnSaturation();

        EXPECT_CALL(*mock_socket, send(_))
                .WillRepeatedly(DoAll(Invoke(this,
                                             &clientside_websocket_tests::copy_output),
                                      Return(true)));

        EXPECT_CALL(*mock_linux_, getrandom(_,_,_))
                .WillOnce(Invoke(getrandom_fake));
        
        EXPECT_CALL(*mock_log_, error(_))
                .Times(AtLeast(1));

        EXPECT_CALL(*mock_log_, warn(_))
                .Times(AtLeast(0));
                                                           
        std::unique_ptr<ISocket> socket = std::move(mock_socket);
        Address remote_address;
        
        // Act
        ASSERT_THROW(std::unique_ptr<ClientSideWebSocket> websocket
                     = make_unique<ClientSideWebSocket>(socket, mock_response_parser_,
                                                        remote_address, mock_linux_,
                                                        mock_log_),
                     std::runtime_error);
}

TEST_F(clientside_websocket_tests, new_client_side_websocket_throws_error_if_not_websocket)
{
        // Arrange
        
        std::unique_ptr<MockSocket> mock_socket = make_unique<MockSocket>();
        
        EXPECT_CALL(mock_response_parser_, parse(_))
                .RetiresOnSaturation();

        EXPECT_CALL(*mock_socket, send(_))
                .WillRepeatedly(DoAll(Invoke(this,
                                             &clientside_websocket_tests::copy_output),
                                      Return(true)));

        EXPECT_CALL(mock_response_parser_, response())
                .WillRepeatedly(ReturnRef(mock_response_));
        
        EXPECT_CALL(mock_response_, assert_websocket(_))
                .WillRepeatedly(ThrowRuntimeException());

        EXPECT_CALL(*mock_linux_, getrandom(_,_,_))
                .WillOnce(Invoke(getrandom_fake));
        
        EXPECT_CALL(*mock_log_, error(_))
                .Times(AtLeast(1));

        EXPECT_CALL(*mock_log_, warn(_))
                .Times(AtLeast(0));

        std::unique_ptr<ISocket> socket = std::move(mock_socket);
        Address remote_address;
        
        // Act
        ASSERT_THROW(std::unique_ptr<ClientSideWebSocket> websocket
                     = make_unique<ClientSideWebSocket>(socket, mock_response_parser_,
                                                        remote_address, mock_linux_,
                                                        mock_log_),
                     std::runtime_error);
}
