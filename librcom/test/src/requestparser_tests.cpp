#include <r.h>
#include "gtest/gtest.h"

#include "../mocks/Socket.mock.h"

#include "RequestParser.h"
#include "Request.h"

using ::testing::Return;
using ::testing::ReturnRef;
using ::testing::_;
using ::testing::SetArgReferee;

using namespace std;
using namespace rcom;
using namespace rpp;

class requestparser_tests : public ::testing::Test
{
public:
        MockSocket socket_;
        rpp::MemBuffer read_data_;
        size_t current_read_char_;

        
protected:
        
        requestparser_tests() : socket_(), read_data_(), current_read_char_(0)
                {}

        ~requestparser_tests() override = default;

        void SetUp() override {
                read_data_.clear();
                EXPECT_CALL(socket_, read(_,_))
                        .WillRepeatedly(DoAll(Invoke(this, &requestparser_tests::socket_read),
                                              Return(true)));
        }

        void TearDown() override {
        }

        void set_read_data(const char *data) {
                read_data_.clear();
                read_data_.append_string(data);
        }

        void set_no_timeout_data() {
                EXPECT_CALL(socket_, wait(_))
                        .WillRepeatedly(Return(kWaitOK));
        }

        void append_read_data(const char *data) {
                read_data_.append_string(data);
        }
        
        bool socket_read(uint8_t *buffer, size_t len) {
                size_t i;
                for (i = 0;
                     i < len && current_read_char_ < read_data_.size();
                     i++, current_read_char_++) {
                        buffer[i] = read_data_.data()[current_read_char_];
                }
                return i == len;
        }
};

TEST_F(requestparser_tests, successfully_parse_request)
{
        // Arrange
        set_no_timeout_data();
        set_read_data("GET / HTTP/1.1\r\n"
                      "Host: 127.0.0.1\r\n"
                      "Connection: Upgrade\r\n"
                      "Upgrade: websocket\r\n"
                      "Sec-WebSocket-Version: 13\r\n"
                      "Sec-WebSocket-Key: AAAAAAAAAAAAAAAAAAAAAA==\r\n"
                      "\r\n");

        Request request;
        RequestParser parser(request);
        
        // Act
        bool success = parser.parse(socket_);
        IRequest& actual_request = parser.request();

        // Assert
        ASSERT_TRUE(success);
        ASSERT_EQ(actual_request.get_method(), IRequest::kGetMethod);
        ASSERT_STREQ(actual_request.get_uri().c_str(), "/");

        std::string value;
        ASSERT_TRUE(actual_request.get_header_value("Host", value));
        ASSERT_STREQ(value.c_str(), "127.0.0.1");
        ASSERT_TRUE(actual_request.get_header_value("Connection", value));
        ASSERT_STREQ(value.c_str(), "Upgrade");
        ASSERT_TRUE(actual_request.get_header_value("Upgrade", value));
        ASSERT_STREQ(value.c_str(), "websocket");
        ASSERT_TRUE(actual_request.get_header_value("Sec-WebSocket-Version", value));
        ASSERT_STREQ(value.c_str(), "13");
        ASSERT_TRUE(actual_request.get_header_value("Sec-WebSocket-Key", value));
        ASSERT_STREQ(value.c_str(), "AAAAAAAAAAAAAAAAAAAAAA==");
}

TEST_F(requestparser_tests, request_with_invalid_method_fails)
{
        // Arrange
        set_no_timeout_data();
        set_read_data("POST / HTTP/1.1\r\n"
                      "Host: 127.0.0.1\r\n"
                      "Connection: Upgrade\r\n"
                      "Upgrade: websocket\r\n"
                      "Sec-WebSocket-Version: 13\r\n"
                      "Sec-WebSocket-Key: AAAAAAAAAAAAAAAAAAAAAA==\r\n"
                      "\r\n");

        Request request;
        RequestParser parser(request);
        
        // Act
        bool success = parser.parse(socket_);

        // Assert
        ASSERT_FALSE(success);
}

TEST_F(requestparser_tests, request_with_invalid_http_version_fails)
{
        // Arrange
        set_no_timeout_data();
        set_read_data("GET / HTTP/2\r\n"
                      "Host: 127.0.0.1\r\n"
                      "Connection: Upgrade\r\n"
                      "Upgrade: websocket\r\n"
                      "Sec-WebSocket-Version: 13\r\n"
                      "Sec-WebSocket-Key: AAAAAAAAAAAAAAAAAAAAAA==\r\n"
                      "\r\n");

        Request request;
        RequestParser parser(request);
        
        // Act
        bool success = parser.parse(socket_);

        // Assert
        ASSERT_FALSE(success);
}

TEST_F(requestparser_tests, request_with_missing_http_version_fails)
{
        // Arrange
        set_no_timeout_data();
        set_read_data("GET /\r\n"
                      "Host: 127.0.0.1\r\n"
                      "\r\n");

        Request request;
        RequestParser parser(request);
        
        // Act
        bool success = parser.parse(socket_);

        // Assert
        ASSERT_FALSE(success);
}

TEST_F(requestparser_tests, request_with_invalid_header_fails_1)
{
        // Arrange
        set_no_timeout_data();
        set_read_data("GET / HTTP/1.1\r\n"
                      "[Host]: 127.0.0.1\r\n"
                      "\r\n");

        Request request;
        RequestParser parser(request);
        
        // Act
        bool success = parser.parse(socket_);

        // Assert
        ASSERT_FALSE(success);
}

TEST_F(requestparser_tests, request_with_invalid_header_fails_2)
{
        // Arrange
        set_no_timeout_data();
        set_read_data("GET / HTTP/1.1\r\n");

        for (size_t i = 0; i < HttpParser::kMaxHeaderNameLength+1; i++)
                append_read_data("A");

        append_read_data(": 127.0.0.1\r\n"
                         "\r\n");

        Request request;
        RequestParser parser(request);
        
        // Act
        bool success = parser.parse(socket_);

        // Assert
        ASSERT_FALSE(success);
}

TEST_F(requestparser_tests, request_with_invalid_header_fails_3)
{
        // Arrange
        set_no_timeout_data();
        set_read_data("GET / HTTP/1.1\r\n"
                      "Name: ");

        for (size_t i = 0; i < HttpParser::kMaxHeaderValueLength+1; i++)
                append_read_data("A");

        append_read_data("\r\n\r\n");

        Request request;
        RequestParser parser(request);
        
        // Act
        bool success = parser.parse(socket_);

        // Assert
        ASSERT_FALSE(success);
}

TEST_F(requestparser_tests, request_with_bad_newlines_fails)
{
        // Arrange
        set_no_timeout_data();
        set_read_data("GET / HTTP/2\n"
                      "Host: 127.0.0.1\n"
                      "\n");

        Request request;
        RequestParser parser(request);
        
        // Act
        bool success = parser.parse(socket_);

        // Assert
        ASSERT_FALSE(success);
}

TEST_F(requestparser_tests, parsing_response_fails)
{
        // Arrange
        set_no_timeout_data();
        set_read_data("HTTP/1.1 101 Switching Protocols\r\n"
                       "Upgrade: websocket\r\n"
                       "Connection: Upgrade\r\n"
                       "Sec-WebSocket-Accept: ICX+Yqv66kxgM0FcWaLWlFLwTAI=\r\n"
                       "\r\n");
        Request request;
        RequestParser parser(request);
        
        // Act
        bool success = parser.parse(socket_);

        // Assert
        ASSERT_FALSE(success);
}
