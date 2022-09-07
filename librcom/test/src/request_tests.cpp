#include "gtest/gtest.h"
#include "rcom/Request.h"
#include "rcom/ConsoleLog.h"

using namespace std;
using namespace rcom;

class request_tests : public ::testing::Test
{
public:
        
protected:
        
        request_tests() = default;
        ~request_tests() override = default;

        void SetUp() override {
        }

        void TearDown() override {
        }
};

TEST_F(request_tests, request_assert_websocket_doesnt_throw)
{
        // Arrange
        Request request;
        request.set_method(IRequest::kGetMethod);
        request.add_header("Connection", "Upgrade");
        request.add_header("Upgrade", "websocket");
        request.add_header("Sec-WebSocket-Key", "AAAAAAAAAAAAAAAAAAAAAA==");
        request.add_header("Sec-WebSocket-Version", "13");
        
        // Act
        // Assert
        ASSERT_NO_THROW(request.assert_websocket());
}

TEST_F(request_tests, request_assert_websocket_throws_on_bad_method)
{
        // Arrange
        Request request;
        request.set_method(IRequest::kUnsupportedMethod);
        request.add_header("Connection", "Upgrade");
        request.add_header("Upgrade", "websocket");
        request.add_header("Sec-WebSocket-Key", "AAAAAAAAAAAAAAAAAAAAAA==");
        request.add_header("Sec-WebSocket-Version", "13");
        
        // Act
        // Assert
        ASSERT_THROW(request.assert_websocket(), std::runtime_error);
}

TEST_F(request_tests, request_assert_websocket_throws_on_missing_header_1)
{
        // Arrange
        Request request;
        request.set_method(IRequest::kGetMethod);
        request.add_header("Upgrade", "websocket");
        request.add_header("Sec-WebSocket-Key", "AAAAAAAAAAAAAAAAAAAAAA==");
        request.add_header("Sec-WebSocket-Version", "13");
        
        // Act
        // Assert
        ASSERT_THROW(request.assert_websocket(), std::runtime_error);
}

TEST_F(request_tests, request_assert_websocket_throws_on_missing_header_2)
{
        // Arrange
        Request request;
        request.set_method(IRequest::kGetMethod);
        request.add_header("Connection", "Upgrade");
        request.add_header("Sec-WebSocket-Key", "AAAAAAAAAAAAAAAAAAAAAA==");
        request.add_header("Sec-WebSocket-Version", "13");
        
        // Act
        // Assert
        ASSERT_THROW(request.assert_websocket(), std::runtime_error);
}

TEST_F(request_tests, request_assert_websocket_throws_on_missing_header_3)
{
        // Arrange
        Request request;
        request.set_method(IRequest::kGetMethod);
        request.add_header("Connection", "Upgrade");
        request.add_header("Upgrade", "websocket");
        request.add_header("Sec-WebSocket-Version", "13");
        
        // Act
        // Assert
        ASSERT_THROW(request.assert_websocket(), std::runtime_error);
}

TEST_F(request_tests, request_assert_websocket_throws_on_missing_header_4)
{
        // Arrange
        Request request;
        request.set_method(IRequest::kGetMethod);
        request.add_header("Connection", "Upgrade");
        request.add_header("Upgrade", "websocket");
        request.add_header("Sec-WebSocket-Key", "AAAAAAAAAAAAAAAAAAAAAA==");
        
        // Act
        // Assert
        ASSERT_THROW(request.assert_websocket(), std::runtime_error);
}

TEST_F(request_tests, request_assert_websocket_throws_on_bad_key_header)
{
        // Arrange
        Request request;
        request.set_method(IRequest::kGetMethod);
        request.add_header("Connection", "Upgrade");
        request.add_header("Upgrade", "websocket");
        request.add_header("Sec-WebSocket-Key", "AAAAAAAAAAAAAAA==");
        request.add_header("Sec-WebSocket-Version", "13");
        
        // Act
        // Assert
        ASSERT_THROW(request.assert_websocket(), std::runtime_error);
}

TEST_F(request_tests, request_assert_websocket_throws_on_bad_version_header)
{
        // Arrange
        Request request;
        request.set_method(IRequest::kGetMethod);
        request.add_header("Connection", "Upgrade");
        request.add_header("Upgrade", "websocket");
        request.add_header("Sec-WebSocket-Key", "AAAAAAAAAAAAAAAAAAAAAA==");
        request.add_header("Sec-WebSocket-Version", "12");
        
        // Act
        // Assert
        ASSERT_THROW(request.assert_websocket(), std::runtime_error);
}
