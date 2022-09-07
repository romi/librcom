#include "gtest/gtest.h"
#include "rcom/Response.h"
#include "rcom/ConsoleLog.h"

using namespace std;
using namespace rcom;

class response_tests : public ::testing::Test
{
public:
        
protected:
        
        response_tests() = default;
        ~response_tests() override = default;

        void SetUp() override {
        }

        void TearDown() override {
        }
};

TEST_F(response_tests, response_assert_websocket_doesnt_throw)
{
        // Arrange
        Response response;
        response.set_code(101);
        response.add_header("Connection", "Upgrade");
        response.add_header("Upgrade", "websocket");
        response.add_header("Sec-WebSocket-Accept", "ICX+Yqv66kxgM0FcWaLWlFLwTAI=");
        
        // Act
        // Assert
        ASSERT_NO_THROW(response.assert_websocket("ICX+Yqv66kxgM0FcWaLWlFLwTAI="));
}

TEST_F(response_tests, response_assert_websocket_throws_on_bad_code)
{
        // Arrange
        Response response;
        response.set_code(200);
        response.add_header("Connection", "Upgrade");
        response.add_header("Upgrade", "websocket");
        response.add_header("Sec-WebSocket-Accept", "ICX+Yqv66kxgM0FcWaLWlFLwTAI=");
        
        // Act
        // Assert
        ASSERT_THROW(response.assert_websocket("ICX+Yqv66kxgM0FcWaLWlFLwTAI="),
                     std::runtime_error);
}

TEST_F(response_tests, response_assert_websocket_throws_on_missing_header_1)
{
        // Arrange
        Response response;
        response.set_code(101);
        response.add_header("Upgrade", "websocket");
        response.add_header("Sec-WebSocket-Accept", "ICX+Yqv66kxgM0FcWaLWlFLwTAI=");
        
        // Act
        // Assert
        ASSERT_THROW(response.assert_websocket("ICX+Yqv66kxgM0FcWaLWlFLwTAI="),
                     std::runtime_error);
}

TEST_F(response_tests, response_assert_websocket_throws_on_missing_header_2)
{
        // Arrange
        Response response;
        response.set_code(101);
        response.add_header("Connection", "Upgrade");
        response.add_header("Sec-WebSocket-Accept", "ICX+Yqv66kxgM0FcWaLWlFLwTAI=");
        
        // Act
        // Assert
        ASSERT_THROW(response.assert_websocket("ICX+Yqv66kxgM0FcWaLWlFLwTAI="),
                     std::runtime_error);
}

TEST_F(response_tests, response_assert_websocket_throws_on_missing_header_3)
{
        // Arrange
        Response response;
        response.set_code(101);
        response.add_header("Connection", "Upgrade");
        response.add_header("Upgrade", "websocket");
        
        // Act
        // Assert
        ASSERT_THROW(response.assert_websocket("ICX+Yqv66kxgM0FcWaLWlFLwTAI="),
                     std::runtime_error);
}

TEST_F(response_tests, response_assert_websocket_throws_on_bad_accept_header)
{
        // Arrange
        Response response;
        response.set_code(101);
        response.add_header("Connection", "Upgrade");
        response.add_header("Upgrade", "websocket");
        response.add_header("Sec-WebSocket-Accept", "ICX+Yqv66kxgM0=");
        
        // Act
        // Assert
        ASSERT_THROW(response.assert_websocket("ICX+Yqv66kxgM0FcWaLWlFLwTAI="),
                     std::runtime_error);
}
