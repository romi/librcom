#include <r.h>
#include "gtest/gtest.h"
#include "Request.h"

//FAKE_VOID_FUNC_VARARG(r_err, const char*, ...)

extern "C" {
#include "log.mock.h"
}

// DECLARE_FAKE_VOID_FUNC_VARARG(r_err, const char*, ...)
// DEFINE_FAKE_VOID_FUNC_VARARG(r_err, const char*, ...)

using namespace std;
using namespace rcom;

class request_tests : public ::testing::Test
{
public:
        
protected:
        
        request_tests() = default;
        ~request_tests() override = default;

        void SetUp() override {
                //RESET_FAKE(r_err);
        }

        void TearDown() override {
        }
};

TEST_F(request_tests, request_is_websocket_returns_true)
{
        // Arrange
        Request request;
        request.set_method(IRequest::kGetMethod);
        request.add_header("Connection", "Upgrade");
        request.add_header("Upgrade", "websocket");
        request.add_header("Sec-WebSocket-Key", "AAAAAAAAAAAAAAAAAAAAAA==");
        request.add_header("Sec-WebSocket-Version", "13");
        
        // Act
        bool success = request.is_websocket();

        // Assert
        ASSERT_TRUE(success);
}

TEST_F(request_tests, request_is_websocket_returns_false_on_bad_method)
{
        // Arrange
        Request request;
        request.set_method(IRequest::kUnsupportedMethod);
        request.add_header("Connection", "Upgrade");
        request.add_header("Upgrade", "websocket");
        request.add_header("Sec-WebSocket-Key", "AAAAAAAAAAAAAAAAAAAAAA==");
        request.add_header("Sec-WebSocket-Version", "13");
        
        // Act
        bool success = request.is_websocket();

        // Assert
        ASSERT_FALSE(success);
        //ASSERT_GE(r_err_fake.call_count, 1);
}

TEST_F(request_tests, request_is_websocket_returns_false_on_missing_header_1)
{
        // Arrange
        Request request;
        request.set_method(IRequest::kGetMethod);
        request.add_header("Upgrade", "websocket");
        request.add_header("Sec-WebSocket-Key", "AAAAAAAAAAAAAAAAAAAAAA==");
        request.add_header("Sec-WebSocket-Version", "13");
        
        // Act
        bool success = request.is_websocket();

        // Assert
        ASSERT_FALSE(success);
        //ASSERT_GE(r_err_fake.call_count, 1);
}

TEST_F(request_tests, request_is_websocket_returns_false_on_missing_header_2)
{
        // Arrange
        Request request;
        request.set_method(IRequest::kGetMethod);
        request.add_header("Connection", "Upgrade");
        request.add_header("Sec-WebSocket-Key", "AAAAAAAAAAAAAAAAAAAAAA==");
        request.add_header("Sec-WebSocket-Version", "13");
        
        // Act
        bool success = request.is_websocket();

        // Assert
        ASSERT_FALSE(success);
        //ASSERT_GE(r_err_fake.call_count, 1);
}

TEST_F(request_tests, request_is_websocket_returns_false_on_missing_header_3)
{
        // Arrange
        Request request;
        request.set_method(IRequest::kGetMethod);
        request.add_header("Connection", "Upgrade");
        request.add_header("Upgrade", "websocket");
        request.add_header("Sec-WebSocket-Version", "13");
        
        // Act
        bool success = request.is_websocket();

        // Assert
        ASSERT_FALSE(success);
        //ASSERT_GE(r_err_fake.call_count, 1);
}

TEST_F(request_tests, request_is_websocket_returns_false_on_missing_header_4)
{
        // Arrange
        Request request;
        request.set_method(IRequest::kGetMethod);
        request.add_header("Connection", "Upgrade");
        request.add_header("Upgrade", "websocket");
        request.add_header("Sec-WebSocket-Key", "AAAAAAAAAAAAAAAAAAAAAA==");
        
        // Act
        bool success = request.is_websocket();

        // Assert
        ASSERT_FALSE(success);
        //ASSERT_GE(r_err_fake.call_count, 1);
}

TEST_F(request_tests, request_is_websocket_returns_false_on_bad_key_header)
{
        // Arrange
        Request request;
        request.set_method(IRequest::kGetMethod);
        request.add_header("Connection", "Upgrade");
        request.add_header("Upgrade", "websocket");
        request.add_header("Sec-WebSocket-Key", "AAAAAAAAAAAAAAA==");
        request.add_header("Sec-WebSocket-Version", "13");
        
        // Act
        bool success = request.is_websocket();

        // Assert
        ASSERT_FALSE(success);
        //ASSERT_GE(r_err_fake.call_count, 1);
}

TEST_F(request_tests, request_is_websocket_returns_false_on_bad_version_header)
{
        // Arrange
        Request request;
        request.set_method(IRequest::kGetMethod);
        request.add_header("Connection", "Upgrade");
        request.add_header("Upgrade", "websocket");
        request.add_header("Sec-WebSocket-Key", "AAAAAAAAAAAAAAAAAAAAAA==");
        request.add_header("Sec-WebSocket-Version", "12");
        
        // Act
        bool success = request.is_websocket();

        // Assert
        ASSERT_FALSE(success);
        //ASSERT_GE(r_err_fake.call_count, 1);
}
