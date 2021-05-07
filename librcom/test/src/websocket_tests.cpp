#include <iostream>
#include <r.h>
#include <ClockAccessor.h>
#include "gtest/gtest.h"

#include "../mocks/Socket.mock.h"
#include "../mocks/RequestParser.mock.h"
#include "../mocks/Request.mock.h"
#include "../mocks/ResponseParser.mock.h"
#include "../mocks/Response.mock.h"
#include "../mocks/Clock.mock.h"

#include "ClientSideWebSocket.h"
#include "ServerSideWebSocket.h"
#include "Address.h"

extern "C" {
#include "../mocks/random.mock.h"
}

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

FAKE_VOID_FUNC_VARARG(r_err, const char*, ...)

class websocket_tests : public ::testing::Test
{
public:
        MockRequestParser mock_request_parser_;
        MockRequest mock_request_;
        MockResponseParser mock_response_parser_;
        MockResponse mock_response_;
        std::shared_ptr<MockClock> mock_clock_;
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
        
        websocket_tests() :
                mock_request_parser_(),
                mock_request_(),
                mock_response_parser_(),
                mock_response_(),
                mock_clock_(),
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

                mock_clock_ = std::make_shared<MockClock>();
                std::shared_ptr<IClock> gClock = mock_clock_;
                rpp::ClockAccessor::SetInstance(gClock);
        }

        ~websocket_tests() override {
                rpp::ClockAccessor::SetInstance(nullptr);
        }

        void SetUp() override {
                RESET_FAKE(r_err);
                output_data_.clear();
                input_data_.clear();
                socket_connected_ = true;
        }

        void TearDown() override {
        }


        void make_server_side_websocket(std::unique_ptr<ServerSideWebSocket>& websocket) {
                // socket
                std::unique_ptr<NiceMock<MockSocket>> mock_socket = make_unique<NiceMock<MockSocket>>();

                EXPECT_CALL(*mock_socket, send(_))
                        .WillRepeatedly(DoAll(Invoke(this, &websocket_tests::copy_output),
                                              Return(true)));

                EXPECT_CALL(*mock_socket, wait(_))
                        .WillRepeatedly(Return(WaitStatus::kWaitOK));

                EXPECT_CALL(*mock_socket, read(_,_))
                        .WillRepeatedly(DoAll(Invoke(this, &websocket_tests::copy_input),
                                              Return(true)));

                EXPECT_CALL(*mock_socket, close())
                        .WillRepeatedly(Assign(&socket_connected_, false));

                EXPECT_CALL(*mock_socket, is_connected())
                        .WillRepeatedly(ReturnPointee(&socket_connected_));

                // parser
                EXPECT_CALL(mock_request_parser_, parse(_))
                        .WillOnce(Return(true))
                        .RetiresOnSaturation();
        
                EXPECT_CALL(mock_request_parser_, request())
                        .WillRepeatedly(ReturnRef(mock_request_));
                
                // clock
                EXPECT_CALL(*mock_clock_, time())
                        .WillRepeatedly(Return(0.0));
                
                // request
                EXPECT_CALL(mock_request_, is_websocket())
                        .WillRepeatedly(Return(true));
        
                EXPECT_CALL(mock_request_, get_header_value("Sec-WebSocket-Key",_))
                        .WillRepeatedly(DoAll(SetArgReferee<1>(request_key_header_value_),
                                              Return(true)));

                
                std::unique_ptr<ISocket> socket = std::move(mock_socket);

                websocket = make_unique<ServerSideWebSocket>(socket, mock_request_parser_);
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
                
        static int fake_r_random(void *buffer, size_t length) {
                memset(buffer, 0, length);
                return 0;
        }
};

TEST_F(websocket_tests, successfull_creation_and_delete_of_server_side_websocket)
{
        std::unique_ptr<ServerSideWebSocket> websocket;
        
        // Arrange        
        input_append_client_1001_close_handshake();

        // Act
        make_server_side_websocket(websocket);
        websocket->close(kCloseGoingAway);
        
        // Assert

        ASSERT_EQ(output_data_.size(), 2); 

        // HTTP handshake
        ASSERT_EQ(output_data_[0].tostring()
                  .find("HTTP/1.1 101 Switching Protocols"), 0);
        ASSERT_NE(output_data_[0].tostring().find("Upgrade: websocket"),
                  std::string::npos);
        ASSERT_NE(output_data_[0].tostring().find("Connection: Upgrade"),
                  std::string::npos);
        ASSERT_NE(output_data_[0].tostring()
                  .find("Sec-WebSocket-Accept: ICX+Yqv66kxgM0FcWaLWlFLwTAI="),
                  std::string::npos);

        // Close message
        ASSERT_EQ(output_data_[1].size(), 4);
        ASSERT_EQ(memcmp(output_data_[1].data().data(), server_1001_close_handshake, 4), 0);
}

TEST_F(websocket_tests, new_server_side_websocket_throws_error_on_failed_parse)
{
        // Arrange
        
        std::unique_ptr<MockSocket> mock_socket = make_unique<MockSocket>();
        
        EXPECT_CALL(mock_request_parser_, parse(_))
                .WillOnce(Return(false))
                .RetiresOnSaturation();
                                                           
        std::unique_ptr<ISocket> socket = std::move(mock_socket);

        // Act
        
        try {
                std::unique_ptr<ServerSideWebSocket> websocket
                        = make_unique<ServerSideWebSocket>(socket, mock_request_parser_);

                FAIL() << "Expected std::runtime_error";
                
        } catch(std::runtime_error const & err) {
                // OK
                
        } catch(...) {
                FAIL() << "Expected std::runtime_error";
        }

        // Assert
        ASSERT_GE(r_err_fake.call_count, 1);
}

TEST_F(websocket_tests, new_server_side_websocket_throws_error_if_not_websocket)
{
        // Arrange
        
        std::unique_ptr<MockSocket> mock_socket = make_unique<MockSocket>();
        
        EXPECT_CALL(mock_request_parser_, parse(_))
                .WillOnce(Return(true))
                .RetiresOnSaturation();
        
        EXPECT_CALL(mock_request_parser_, request())
                .WillRepeatedly(ReturnRef(mock_request_));
        
        EXPECT_CALL(mock_request_, is_websocket())
                .WillRepeatedly(Return(false));
                                                           
        std::unique_ptr<ISocket> socket = std::move(mock_socket);

        // Act
        
        try {
                std::unique_ptr<ServerSideWebSocket> websocket
                        = make_unique<ServerSideWebSocket>(socket, mock_request_parser_);

                FAIL() << "Expected std::runtime_error";
                
        } catch(std::runtime_error const & err) {
                // OK
                
        } catch(...) {
                FAIL() << "Expected std::runtime_error";
        }

        // Assert
        ASSERT_GE(r_err_fake.call_count, 1);
}

TEST_F(websocket_tests, new_server_side_websocket_throws_error_if_send_fails)
{
        // Arrange
        
        std::unique_ptr<MockSocket> mock_socket = make_unique<MockSocket>();
        
        EXPECT_CALL(mock_request_parser_, parse(_))
                .WillOnce(Return(true))
                .RetiresOnSaturation();
        
        EXPECT_CALL(mock_request_parser_, request())
                .WillRepeatedly(ReturnRef(mock_request_));
        
        EXPECT_CALL(mock_request_, is_websocket())
                .WillRepeatedly(Return(true));
        
        EXPECT_CALL(mock_request_, get_header_value("Sec-WebSocket-Key",_))
                .WillRepeatedly(DoAll(SetArgReferee<1>(request_key_header_value_),
                                      Return(true)));

        EXPECT_CALL(*mock_socket, send(_))
                .WillOnce(Return(false));
        
        EXPECT_CALL(*mock_socket, close());
                                                           
        std::unique_ptr<ISocket> socket = std::move(mock_socket);

        // Act
        
        try {
                std::unique_ptr<ServerSideWebSocket> websocket
                        = make_unique<ServerSideWebSocket>(socket, mock_request_parser_);

                FAIL() << "Expected std::runtime_error";
                
        } catch(std::runtime_error const & err) {
                // OK
                
        } catch(...) {
                FAIL() << "Expected std::runtime_error";
        }

        // Assert
        ASSERT_GE(r_err_fake.call_count, 1);
}

TEST_F(websocket_tests, new_server_side_websocket_throws_error_if_read_fails)
{
        // Arrange
        
        std::unique_ptr<MockSocket> mock_socket = make_unique<MockSocket>();
        
        EXPECT_CALL(mock_request_parser_, parse(_))
                .WillOnce(Return(true))
                .RetiresOnSaturation();
        
        EXPECT_CALL(mock_request_parser_, request())
                .WillRepeatedly(ReturnRef(mock_request_));
        
        EXPECT_CALL(mock_request_, is_websocket())
                .WillRepeatedly(Return(true));
        
        EXPECT_CALL(mock_request_, get_header_value("Sec-WebSocket-Key",_))
                .WillRepeatedly(DoAll(SetArgReferee<1>(request_key_header_value_),
                                      Return(true)));

        EXPECT_CALL(*mock_socket, send(_))
                .WillOnce(Return(false));
        
        EXPECT_CALL(*mock_socket, wait(_))
                .WillRepeatedly(Return(WaitStatus::kWaitOK));

        EXPECT_CALL(*mock_socket, read(_,_))
                .WillRepeatedly(DoAll(Invoke(this, &websocket_tests::copy_input),
                                      Return(true)));
        
        EXPECT_CALL(*mock_socket, close());
                                                           
        std::unique_ptr<ISocket> socket = std::move(mock_socket);

        // Act
        
        try {
                std::unique_ptr<ServerSideWebSocket> websocket
                        = make_unique<ServerSideWebSocket>(socket, mock_request_parser_);

                FAIL() << "Expected std::runtime_error";
                
        } catch(std::runtime_error const & err) {
                // OK
                
        } catch(...) {
                FAIL() << "Expected std::runtime_error";
        }

        // Assert
        ASSERT_GE(r_err_fake.call_count, 1);
}

TEST_F(websocket_tests, successfull_creation_and_delete_of_client_side_websocket)
{
        // Arrange
        
        std::unique_ptr<MockSocket> mock_socket = make_unique<MockSocket>();
        
        EXPECT_CALL(mock_response_parser_, parse(_))
                .WillOnce(Return(true))
                .RetiresOnSaturation();
        
        EXPECT_CALL(mock_response_parser_, response())
                .WillRepeatedly(ReturnRef(mock_response_));
        
        EXPECT_CALL(mock_response_, is_websocket(_))
                .WillRepeatedly(Return(true));

        EXPECT_CALL(*mock_socket, send(_))
                .WillRepeatedly(DoAll(Invoke(this, &websocket_tests::copy_output),
                                      Return(true)));

        EXPECT_CALL(*mock_socket, wait(_))
                .WillRepeatedly(Return(WaitStatus::kWaitOK));

        EXPECT_CALL(*mock_socket, read(_,_))
                .WillRepeatedly(DoAll(Invoke(this, &websocket_tests::copy_input),
                                      Return(true)));
        
        EXPECT_CALL(*mock_socket, close());
                
        EXPECT_CALL(*mock_clock_, sleep(_));
        EXPECT_CALL(*mock_clock_, time())
                .WillRepeatedly(Return(0.0));

        r_random_fake.custom_fake = websocket_tests::fake_r_random;
        
        input_append_server_1001_close_handshake();
                                                           
        std::unique_ptr<ISocket> socket = std::move(mock_socket);
        Address remote_address;
        
        // Act
        std::unique_ptr<ClientSideWebSocket> websocket
                = make_unique<ClientSideWebSocket>(socket, mock_response_parser_, remote_address);
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

TEST_F(websocket_tests, new_client_side_websocket_throws_error_if_send_fails)
{
        // Arrange
        
        std::unique_ptr<MockSocket> mock_socket = make_unique<MockSocket>();

        EXPECT_CALL(*mock_socket, send(_))
                .WillOnce(Return(false));

        EXPECT_CALL(*mock_socket, close());
        EXPECT_CALL(*mock_clock_, sleep(_));
        
        r_random_fake.custom_fake = websocket_tests::fake_r_random;
                                                           
        std::unique_ptr<ISocket> socket = std::move(mock_socket);
        Address remote_address;
        
        // Act
        try {
                std::unique_ptr<ClientSideWebSocket> websocket
                        = make_unique<ClientSideWebSocket>(socket, mock_response_parser_,
                                                           remote_address);

                FAIL() << "Expected std::runtime_error";
                
        } catch(std::runtime_error const & err) {
                // OK
                
        } catch(...) {
                FAIL() << "Expected std::runtime_error";
        }

        // Assert
        ASSERT_GE(r_err_fake.call_count, 1);
}

TEST_F(websocket_tests, new_client_side_websocket_throws_error_on_failed_parse)
{
        // Arrange
        
        std::unique_ptr<MockSocket> mock_socket = make_unique<MockSocket>();
        
        EXPECT_CALL(mock_response_parser_, parse(_))
                .WillOnce(Return(false))
                .RetiresOnSaturation();

        EXPECT_CALL(*mock_socket, send(_))
                .WillRepeatedly(DoAll(Invoke(this, &websocket_tests::copy_output),
                                      Return(true)));

        r_random_fake.custom_fake = websocket_tests::fake_r_random;
                                                           
        std::unique_ptr<ISocket> socket = std::move(mock_socket);
        Address remote_address;
        
        // Act
        try {
                std::unique_ptr<ClientSideWebSocket> websocket
                        = make_unique<ClientSideWebSocket>(socket, mock_response_parser_,
                                                           remote_address);

                FAIL() << "Expected std::runtime_error";
                
        } catch(std::runtime_error const & err) {
                // OK
                
        } catch(...) {
                FAIL() << "Expected std::runtime_error";
        }

        // Assert
        ASSERT_GE(r_err_fake.call_count, 1);
}

TEST_F(websocket_tests, new_client_side_websocket_throws_error_if_not_websocket)
{
        // Arrange
        
        std::unique_ptr<MockSocket> mock_socket = make_unique<MockSocket>();
        
        EXPECT_CALL(mock_response_parser_, parse(_))
                .WillOnce(Return(true))
                .RetiresOnSaturation();

        EXPECT_CALL(*mock_socket, send(_))
                .WillRepeatedly(DoAll(Invoke(this, &websocket_tests::copy_output),
                                      Return(true)));

        EXPECT_CALL(mock_response_parser_, response())
                .WillRepeatedly(ReturnRef(mock_response_));
        
        EXPECT_CALL(mock_response_, is_websocket(_))
                .WillRepeatedly(Return(false));

        r_random_fake.custom_fake = websocket_tests::fake_r_random;
                                                           
        std::unique_ptr<ISocket> socket = std::move(mock_socket);
        Address remote_address;
        
        // Act
        try {
                std::unique_ptr<ClientSideWebSocket> websocket
                        = make_unique<ClientSideWebSocket>(socket, mock_response_parser_,
                                                           remote_address);

                FAIL() << "Expected std::runtime_error";
                
        } catch(std::runtime_error const & err) {
                // OK
                
        } catch(...) {
                FAIL() << "Expected std::runtime_error";
        }

        // Assert
        ASSERT_GE(r_err_fake.call_count, 1);
}

TEST_F(websocket_tests, read_text_message_correctly)
{
        MemBuffer message;
        std::unique_ptr<ServerSideWebSocket> websocket;
        make_server_side_websocket(websocket);
        
        // Arrange
        input_append(masked_text_message, sizeof(masked_text_message));

        // Act
        bool success = websocket->recv(message);

        // Assert
        ASSERT_TRUE(success);
        ASSERT_STREQ(message.tostring().c_str(), "abc");
}

TEST_F(websocket_tests, read_binary_message_correctly)
{
        MemBuffer message;
        std::unique_ptr<ServerSideWebSocket> websocket;
        make_server_side_websocket(websocket);
        
        // Arrange
        input_append(masked_binary_message, sizeof(masked_binary_message));

        // Act
        bool success = websocket->recv(message);

        // Assert
        ASSERT_TRUE(success);
        ASSERT_STREQ(message.tostring().c_str(), "abc");
}

TEST_F(websocket_tests, read_fragmented_message_correctly)
{
        MemBuffer message;
        std::unique_ptr<ServerSideWebSocket> websocket;
        make_server_side_websocket(websocket);
        
        // Arrange
        input_append(fragmented_message_a, sizeof(fragmented_message_a));
        input_append(fragmented_message_b, sizeof(fragmented_message_b));
        input_append(fragmented_message_c, sizeof(fragmented_message_c));

        // Act
        RecvStatus status = websocket->recv(message, 1.0);

        // Assert
        ASSERT_EQ(status, kRecvText);
        ASSERT_STREQ(message.tostring().c_str(), "abc");
}

TEST_F(websocket_tests, invalid_fragmented_message_returns_error_1)
{
        MemBuffer message;
        std::unique_ptr<ServerSideWebSocket> websocket;
        make_server_side_websocket(websocket);
        
        // Arrange
        input_append(fragmented_message_a, sizeof(fragmented_message_a));
        // The second TEXT message follows a the non-terminated first
        // message.
        input_append(fragmented_message_a, sizeof(fragmented_message_a));        
        input_append(client_close_reply, sizeof(client_close_reply));

        // Act
        RecvStatus status = websocket->recv(message, 1.0);

        // ASSERT
        ASSERT_EQ(status, kRecvError);
        ASSERT_EQ(output_data_.size(), 2);
        MemBuffer& close_message = output_data_[1];
        ASSERT_EQ(close_message.size(), sizeof(server_1002_close_handshake));
        ASSERT_TRUE(memcmp(close_message.data().data(), server_1002_close_handshake,
                           sizeof(server_1002_close_handshake)) == 0);
}

TEST_F(websocket_tests, invalid_fragmented_message_returns_error_2)
{
        MemBuffer message;
        std::unique_ptr<ServerSideWebSocket> websocket;
        make_server_side_websocket(websocket);

        // Arrange
        input_append(fragmented_message_a, sizeof(fragmented_message_a));
        input_append(fragmented_message_b, sizeof(fragmented_message_b));
        input_append(fragmented_message_a, sizeof(fragmented_message_a));
        input_append(client_close_reply, sizeof(client_close_reply));

        // Act
        RecvStatus status = websocket->recv(message, 1.0);

        // Assert
        ASSERT_EQ(status, kRecvError);
        ASSERT_EQ(output_data_.size(), 2);
        MemBuffer& close_message = output_data_[1];
        ASSERT_EQ(close_message.size(), sizeof(server_1002_close_handshake));
        ASSERT_TRUE(memcmp(close_message.data().data(), server_1002_close_handshake,
                           sizeof(server_1002_close_handshake)) == 0);
}

TEST_F(websocket_tests, read_message_of_size_126)
{
        MemBuffer message;
        std::unique_ptr<ServerSideWebSocket> websocket;
        make_server_side_websocket(websocket);

        // Arrange
        input_append(binary_frame_len126, sizeof(binary_frame_len126));
        input_append_random_data(126);

        // Act
        RecvStatus status = websocket->recv(message, 1.0);

        // Assert
        ASSERT_EQ(status, kRecvBinary);
        ASSERT_EQ(message.size(), 126);
}

TEST_F(websocket_tests, read_message_of_size_65536)
{
        MemBuffer message;
        std::unique_ptr<ServerSideWebSocket> websocket;
        make_server_side_websocket(websocket);
        
        // Arrange
        input_append(binary_frame_len65536, sizeof(binary_frame_len65536));
        input_append_random_data(65536);

        // Act
        RecvStatus status = websocket->recv(message, 1.0);

        // Assert
        ASSERT_EQ(status, kRecvBinary);
        ASSERT_EQ(message.size(), 65536);
}

TEST_F(websocket_tests, read_too_long_a_message)
{
        MemBuffer message;
        std::unique_ptr<ServerSideWebSocket> websocket;
        make_server_side_websocket(websocket);
        
        // Arrange

        // FIN(0x80)|BINARY(0x02) + MASK(0x80)|LEN(127=0x7f) + LEN(?) + MASK(4)
        uint8_t binary_frame_len_too_long[] = { 0x82, 0xff,
                                                0x00, 0x00, 0x00, 0x00,
                                                0x00, 0x00, 0x00, 0x00,
                                                0x00, 0x00, 0x00, 0x00 };
        
        uint64_t too_long = WebSocket::kMaximumPayloadLength + 1;
        binary_frame_len_too_long[2] = (uint8_t) ((too_long & 0xff00000000000000) >> 56);
        binary_frame_len_too_long[3] = (uint8_t) ((too_long & 0x00ff000000000000) >> 48);
        binary_frame_len_too_long[4] = (uint8_t) ((too_long & 0x0000ff0000000000) >> 40);
        binary_frame_len_too_long[5] = (uint8_t) ((too_long & 0x000000ff00000000) >> 32);
        binary_frame_len_too_long[6] = (uint8_t) ((too_long & 0x00000000ff000000) >> 24);
        binary_frame_len_too_long[7] = (uint8_t) ((too_long & 0x0000000000ff0000) >> 16);
        binary_frame_len_too_long[8] = (uint8_t) ((too_long & 0x000000000000ff00) >> 8);
        binary_frame_len_too_long[9] = (uint8_t) (too_long & 0x00000000000000ff);

        input_append(binary_frame_len_too_long, sizeof(binary_frame_len_too_long));

        // Act
        RecvStatus status = websocket->recv(message, 1.0);

        // Assert
        ASSERT_EQ(status, kRecvError);
        ASSERT_EQ(output_data_.size(), 2);
        MemBuffer& close_message = output_data_[1];
        ASSERT_EQ(close_message.size(), sizeof(server_1009_close_handshake));
        ASSERT_TRUE(memcmp(close_message.data().data(), server_1009_close_handshake,
                           sizeof(server_1009_close_handshake)) == 0);
}

TEST_F(websocket_tests, unmasked_client_message_returns_error)
{
        MemBuffer message;
        std::unique_ptr<ServerSideWebSocket> websocket;
        make_server_side_websocket(websocket);

        // Arrange
        input_append(unmasked_text_message, sizeof(unmasked_text_message));

        // Act
        RecvStatus status = websocket->recv(message, 1.0);

        // Assert
        ASSERT_EQ(status, kRecvError);
        ASSERT_EQ(output_data_.size(), 2);
        MemBuffer& close_message = output_data_[1];
        ASSERT_EQ(close_message.size(), sizeof(server_1002_close_handshake));
        ASSERT_TRUE(memcmp(close_message.data().data(), server_1002_close_handshake,
                           sizeof(server_1002_close_handshake)) == 0);
}

TEST_F(websocket_tests, bad_message_opcode_returns_error)
{
        MemBuffer message;
        std::unique_ptr<ServerSideWebSocket> websocket;
        make_server_side_websocket(websocket);

        // Arrange

        // FIN(0x80)|???(0x07) + MASK(0x80)|LEN(3) + MASK(4) + DATA('abc')
        static constexpr const uint8_t invalid_message[] = { 0x87, 0x83,
                                                             0x00, 0x00, 0x00, 0x00,
                                                             'a', 'b', 'c' };
        input_append(invalid_message, sizeof(invalid_message));

        // Act
        RecvStatus status = websocket->recv(message, 1.0);

        // Assert
        ASSERT_EQ(status, kRecvError);
        ASSERT_EQ(output_data_.size(), 2);
        MemBuffer& close_message = output_data_[1];
        ASSERT_EQ(close_message.size(), sizeof(server_1002_close_handshake));
        ASSERT_TRUE(memcmp(close_message.data().data(),
                           server_1002_close_handshake,
                           sizeof(server_1002_close_handshake)) == 0);
}

TEST_F(websocket_tests, politely_replies_to_close_handshake)
{
        MemBuffer message;
        std::unique_ptr<ServerSideWebSocket> websocket;
        make_server_side_websocket(websocket);

        // Arrange
        input_append(client_1001_close_handshake, sizeof(client_1001_close_handshake));

        // Act
        RecvStatus status = websocket->recv(message, 1.0);

        // Assert
        ASSERT_EQ(status, kRecvClosed);
        ASSERT_EQ(output_data_.size(), 2);
        MemBuffer& close_message = output_data_[1];
        ASSERT_EQ(close_message.size(), sizeof(server_1001_close_handshake));
        ASSERT_TRUE(memcmp(close_message.data().data(),
                           server_1001_close_handshake,
                           sizeof(server_1001_close_handshake)) == 0);
}

TEST_F(websocket_tests, send_message_succesfully)
{
        MemBuffer message;
        std::unique_ptr<ServerSideWebSocket> websocket;
        make_server_side_websocket(websocket);

        // Arrange
        message.append_string("abc");

        // Act
        bool success = websocket->send(message);
        
        // Assert
        ASSERT_TRUE(success);
        ASSERT_EQ(output_data_.size(), 2);
        MemBuffer& actual_message = output_data_[1];
        ASSERT_EQ(actual_message.size(), sizeof(unmasked_text_message));
        ASSERT_EQ(memcmp(actual_message.data().data(),
                         unmasked_text_message,
                         sizeof(unmasked_text_message)), 0);
}

