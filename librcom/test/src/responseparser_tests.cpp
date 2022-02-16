#include "gtest/gtest.h"

#include "Socket.mock.h"

#include "ResponseParser.h"
#include "Response.h"

using ::testing::Return;
using ::testing::ReturnRef;
using ::testing::_;
using ::testing::SetArgReferee;

using namespace std;
using namespace rcom;
//using namespace rpp;

class responseparser_tests : public ::testing::Test
{
public:
        MockSocket socket_;
        rcom::MemBuffer read_data_;
        size_t current_read_char_;

        
protected:
        
        responseparser_tests() : socket_(), read_data_(), current_read_char_(0)
                {}

        ~responseparser_tests() override = default;

        void SetUp() override {
                read_data_.clear();
                EXPECT_CALL(socket_, read(_,_))
                        .WillRepeatedly(DoAll(Invoke(this, &responseparser_tests::socket_read),
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

TEST_F(responseparser_tests, successfully_parse_response)
{
        // Arrange
        set_no_timeout_data();
        set_read_data("HTTP/1.1 101 Switching Protocols\r\n"
                      "Upgrade: websocket\r\n"
                      "Connection: Upgrade\r\n"
                      "Sec-WebSocket-Accept: ICX+Yqv66kxgM0FcWaLWlFLwTAI=\r\n"
                      "\r\n");

        Response response;
        ResponseParser parser(response);
        
        // Act
        bool success = parser.parse(socket_);
        IResponse& actual_response = parser.response();

        // Assert
        ASSERT_TRUE(success);
        ASSERT_EQ(actual_response.get_code(), 101);

        std::string value;
        ASSERT_TRUE(actual_response.get_header_value("Connection", value));
        ASSERT_STREQ(value.c_str(), "Upgrade");
        ASSERT_TRUE(actual_response.get_header_value("Upgrade", value));
        ASSERT_STREQ(value.c_str(), "websocket");
        ASSERT_TRUE(actual_response.get_header_value("Sec-WebSocket-Accept", value));
        ASSERT_STREQ(value.c_str(), "ICX+Yqv66kxgM0FcWaLWlFLwTAI=");
}

TEST_F(responseparser_tests, response_with_invalid_http_version_fails)
{
        // Arrange
        set_no_timeout_data();
        set_read_data("HTTP/2 101 Switching Protocols\r\n"
                      "Upgrade: websocket\r\n"
                      "Connection: Upgrade\r\n"
                      "Sec-WebSocket-Accept: ICX+Yqv66kxgM0FcWaLWlFLwTAI=\r\n"
                      "\r\n");

        Response response;
        ResponseParser parser(response);
        
        // Act
        bool success = parser.parse(socket_);

        // Assert
        ASSERT_FALSE(success);
}

TEST_F(responseparser_tests, response_with_missing_http_version_fails)
{
        // Arrange
        set_no_timeout_data();
        set_read_data("101 Switching Protocols\r\n"
                       "Upgrade: websocket\r\n"
                       "Connection: Upgrade\r\n"
                       "Sec-WebSocket-Accept: ICX+Yqv66kxgM0FcWaLWlFLwTAI=\r\n"
                       "\r\n");

        Response response;
        ResponseParser parser(response);
        
        // Act
        bool success = parser.parse(socket_);

        // Assert
        ASSERT_FALSE(success);
}

TEST_F(responseparser_tests, response_with_missing_status_code_fails)
{
        // Arrange
        set_no_timeout_data();
        set_read_data("HTTP/1.1 Switching Protocols\r\n"
                       "Upgrade: websocket\r\n"
                       "Connection: Upgrade\r\n"
                       "Sec-WebSocket-Accept: ICX+Yqv66kxgM0FcWaLWlFLwTAI=\r\n"
                       "\r\n");

        Response response;
        ResponseParser parser(response);
        
        // Act
        bool success = parser.parse(socket_);

        // Assert
        ASSERT_FALSE(success);
}

TEST_F(responseparser_tests, response_with_missing_reason_fails)
{
        // Arrange
        set_no_timeout_data();
        set_read_data("HTTP/1.1 101\r\n"
                       "Upgrade: websocket\r\n"
                       "Connection: Upgrade\r\n"
                       "Sec-WebSocket-Accept: ICX+Yqv66kxgM0FcWaLWlFLwTAI=\r\n"
                       "\r\n");

        Response response;
        ResponseParser parser(response);
        
        // Act
        bool success = parser.parse(socket_);

        // Assert
        ASSERT_FALSE(success);
}

TEST_F(responseparser_tests, response_with_invalid_header_fails_1)
{
        // Arrange
        set_no_timeout_data();
        set_read_data("HTTP/1.1 101 Switching Protocols\r\n"
                      "[Host]: 127.0.0.1\r\n"
                      "\r\n");

        Response response;
        ResponseParser parser(response);
        
        // Act
        bool success = parser.parse(socket_);

        // Assert
        ASSERT_FALSE(success);
}

TEST_F(responseparser_tests, response_with_invalid_header_fails_2)
{
        // Arrange
        set_no_timeout_data();
        set_read_data("HTTP/1.1 101 Switching Protocols\r\n");

        for (size_t i = 0; i < HttpParser::kMaxHeaderNameLength+1; i++)
                append_read_data("A");

        append_read_data(": Value\r\n"
                         "\r\n");

        Response response;
        ResponseParser parser(response);
        
        // Act
        bool success = parser.parse(socket_);

        // Assert
        ASSERT_FALSE(success);
}

TEST_F(responseparser_tests, response_with_invalid_header_fails_3)
{
        // Arrange
        set_no_timeout_data();
        set_read_data("HTTP/1.1 101 Switching Protocols\r\n"
                      "Name: ");

        for (size_t i = 0; i < HttpParser::kMaxHeaderValueLength+1; i++)
                append_read_data("A");

        append_read_data("\r\n\r\n");

        Response response;
        ResponseParser parser(response);
        
        // Act
        bool success = parser.parse(socket_);

        // Assert
        ASSERT_FALSE(success);
}

TEST_F(responseparser_tests, response_with_bad_newlines_fails)
{
        // Arrange
        set_no_timeout_data();
        set_read_data("HTTP/1.1 101 Switching Protocols\n"
                      "Name: value\n"
                      "\n");

        Response response;
        ResponseParser parser(response);
        
        // Act
        bool success = parser.parse(socket_);

        // Assert
        ASSERT_FALSE(success);
}

TEST_F(responseparser_tests, parsing_response_fails)
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
        
        Response response;
        ResponseParser parser(response);
        
        // Act
        bool success = parser.parse(socket_);

        // Assert
        ASSERT_FALSE(success);
}
