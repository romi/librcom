#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include "rcom/RcomMessageHandler.h"

#include "WebSocket.mock.h"
#include "RPCHandler.mock.h"
#include "WebSocketServer.mock.h"

using namespace std;
using namespace testing;
using namespace rcom;

class rcommessagehandler_tests : public ::testing::Test
{
protected:
        MockWebSocketServer ws_server_;
        MockRPCHandler handler_;
        MockWebSocket websocket_;
        std::string output_;
        RPCError return_error_;
        std::string sent_method_;
        nlohmann::json sent_params_;
        nlohmann::json result_;
        
	rcommessagehandler_tests()
                : ws_server_(),
                  handler_(),
                  websocket_(),
                  output_(),
                  return_error_(),
                  sent_method_(),
                  sent_params_(),
                  result_() {
        }

	~rcommessagehandler_tests() override = default;

	void SetUp() override {
                output_ = "";
                return_error_.code = 0;
                return_error_.message = "";
                sent_method_ = "";
//                sent_params_;
//                result_;
        }

	void TearDown() override {}

public:
        
        bool send(rcom::MemBuffer& message, rcom::MessageType type) {
                (void) type;
                output_ = message.tostring();
//                r_err("send: %s", output_.c_str());
                return true;
        }

        int error_code() {
                nlohmann::json reply = nlohmann::json::parse(output_.c_str());
                return (int) reply["error"]["code"];
        }

        nlohmann::json get_result() {
                nlohmann::json reply = nlohmann::json::parse(output_.c_str());
                return reply["result"];
        }

        void arrange_error(int16_t code, const char *message) {
                return_error_.code = code;
                return_error_.message = message;
        }

        void set_error(const std::string& method,
                       nlohmann::json& params,
                       nlohmann::json& result,
                       RPCError& error) {
                (void) result;
//                r_debug("method=%s", method.c_str());
                sent_method_ = method;
                sent_params_ = params;
                error.code = return_error_.code;
                error.message = return_error_.message;
        }
        
        void set_result(const std::string& method,
                        nlohmann::json& params,
                        nlohmann::json& result,
                        RPCError& error) {
//                r_debug("method=%s", method.c_str());
                sent_method_ = method;
                sent_params_ = params;
                error.code = 0;
                result = result_;
        }
};

TEST_F(rcommessagehandler_tests, request_with_invalid_json_returns_appropriate_error_code)
{
        // Arrange
        RcomMessageHandler message_handler(handler_);
        rcom::MemBuffer message;
        message.printf("this is invalid json");

        EXPECT_CALL(websocket_, send(_,_))
                .WillOnce(DoAll(Invoke(this, &rcommessagehandler_tests::send),
                                Return(true)));
        
        // Act
        message_handler.onmessage(ws_server_, websocket_, message, rcom::kTextMessage);

        // Assert
        ASSERT_EQ(error_code(), RPCError::kParseError);
}

TEST_F(rcommessagehandler_tests, request_with_missing_method_returns_appropriate_error_code)
{
        // Arrange
        RcomMessageHandler message_handler(handler_);
        rcom::MemBuffer message;
        message.printf("{}");

        EXPECT_CALL(websocket_, send(_,_))
                .WillOnce(DoAll(Invoke(this, &rcommessagehandler_tests::send),
                                Return(true)));
        
        // Act
        message_handler.onmessage(ws_server_, websocket_, message, rcom::kTextMessage);

        // Assert
        ASSERT_EQ(error_code(), RPCError::kInvalidRequest);
}

TEST_F(rcommessagehandler_tests, request_with_unknown_method_returns_appropriate_error_code)
{
        // Arrange
        RcomMessageHandler message_handler(handler_);
        rcom::MemBuffer message;
        message.printf("{\"method\": \"toto\"}");
        arrange_error(RPCError::kMethodNotFound, "MESSAGE");

        EXPECT_CALL(handler_, execute(_,_,An<nlohmann::json&>(),_))
                .WillOnce(Invoke(this, &rcommessagehandler_tests::set_error));
        
        EXPECT_CALL(websocket_, send(_,_))
                .WillOnce(DoAll(Invoke(this, &rcommessagehandler_tests::send),
                                Return(true)));
        
        // Act
        message_handler.onmessage(ws_server_, websocket_, message, rcom::kTextMessage);

        // Assert
        ASSERT_EQ(error_code(), RPCError::kMethodNotFound);
}

TEST_F(rcommessagehandler_tests, response_contains_expected_result)
{
        // Arrange
        RcomMessageHandler message_handler(handler_);
        rcom::MemBuffer message;
        message.printf("{\"method\": \"toto\"}");
        result_ = nlohmann::json::parse("{\"key\": \"test value\"}");

        EXPECT_CALL(handler_, execute(_,_,An<nlohmann::json&>(),_))
                .WillOnce(Invoke(this, &rcommessagehandler_tests::set_result));
        
        EXPECT_CALL(websocket_, send(_,_))
                .WillOnce(DoAll(Invoke(this, &rcommessagehandler_tests::send),
                                Return(true)));
        
        // Act
        message_handler.onmessage(ws_server_, websocket_, message, rcom::kTextMessage);

        // Assert
        nlohmann::json received_result = get_result();
        ASSERT_EQ(received_result["key"], "test value");
}

