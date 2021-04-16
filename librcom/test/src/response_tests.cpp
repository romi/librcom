#include <r.h>
#include "gtest/gtest.h"
#include "Response.h"

//FAKE_VOID_FUNC_VARARG(r_err, const char*, ...)

// extern "C" {
// #include "log.mock.h"
// }

// DECLARE_FAKE_VOID_FUNC_VARARG(r_err, const char*, ...)
// DEFINE_FAKE_VOID_FUNC_VARARG(r_err, const char*, ...)

using namespace std;
using namespace rcom;

class response_tests : public ::testing::Test
{
public:
        
protected:
        
        response_tests() = default;
        ~response_tests() override = default;

        void SetUp() override {
                //RESET_FAKE(r_err);
        }

        void TearDown() override {
        }
};

TEST_F(response_tests, response_is_websocket_returns_true)
{
        // Arrange
        Response response;
        response.set_code(101);
        response.add_header("Connection", "Upgrade");
        response.add_header("Upgrade", "websocket");
        response.add_header("Sec-WebSocket-Accept", "ICX+Yqv66kxgM0FcWaLWlFLwTAI=");
        
        // Act
        bool success = response.is_websocket("ICX+Yqv66kxgM0FcWaLWlFLwTAI=");

        // Assert
        ASSERT_TRUE(success);
}

TEST_F(response_tests, response_is_websocket_returns_false_on_bad_code)
{
        // Arrange
        Response response;
        response.set_code(200);
        response.add_header("Connection", "Upgrade");
        response.add_header("Upgrade", "websocket");
        response.add_header("Sec-WebSocket-Accept", "ICX+Yqv66kxgM0FcWaLWlFLwTAI=");
        
        // Act
        bool success = response.is_websocket("ICX+Yqv66kxgM0FcWaLWlFLwTAI=");

        // Assert
        ASSERT_FALSE(success);
        //ASSERT_GE(r_err_fake.call_count, 1);
}

TEST_F(response_tests, response_is_websocket_returns_false_on_missing_header_1)
{
        // Arrange
        Response response;
        response.set_code(101);
        response.add_header("Upgrade", "websocket");
        response.add_header("Sec-WebSocket-Accept", "ICX+Yqv66kxgM0FcWaLWlFLwTAI=");
        
        // Act
        bool success = response.is_websocket("ICX+Yqv66kxgM0FcWaLWlFLwTAI=");

        // Assert
        ASSERT_FALSE(success);
        //ASSERT_GE(r_err_fake.call_count, 1);
}

TEST_F(response_tests, response_is_websocket_returns_false_on_missing_header_2)
{
        // Arrange
        Response response;
        response.set_code(101);
        response.add_header("Connection", "Upgrade");
        response.add_header("Sec-WebSocket-Accept", "ICX+Yqv66kxgM0FcWaLWlFLwTAI=");
        
        // Act
        bool success = response.is_websocket("ICX+Yqv66kxgM0FcWaLWlFLwTAI=");

        // Assert
        ASSERT_FALSE(success);
        //ASSERT_GE(r_err_fake.call_count, 1);
}

TEST_F(response_tests, response_is_websocket_returns_false_on_missing_header_3)
{
        // Arrange
        Response response;
        response.set_code(101);
        response.add_header("Connection", "Upgrade");
        response.add_header("Upgrade", "websocket");
        
        // Act
        bool success = response.is_websocket("ICX+Yqv66kxgM0FcWaLWlFLwTAI=");

        // Assert
        ASSERT_FALSE(success);
        //ASSERT_GE(r_err_fake.call_count, 1);
}

TEST_F(response_tests, response_is_websocket_returns_false_on_bad_accept_header)
{
        // Arrange
        Response response;
        response.set_code(101);
        response.add_header("Connection", "Upgrade");
        response.add_header("Upgrade", "websocket");
        response.add_header("Sec-WebSocket-Accept", "ICX+Yqv66kxgM0=");
        
        // Act
        bool success = response.is_websocket("ICX+Yqv66kxgM0FcWaLWlFLwTAI=");

        // Assert
        ASSERT_FALSE(success);
        //ASSERT_GE(r_err_fake.call_count, 1);
}
