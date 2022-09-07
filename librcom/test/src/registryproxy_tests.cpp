#include <string>
#include "gtest/gtest.h"

#include "rcom/RegistryProxy.h"

#include "WebSocket.mock.h"
#include "Linux.mock.h"
#include "Log.mock.h"

using namespace rcom;

using ::testing::_;
using ::testing::Return;

class registryproxy_tests : public ::testing::Test
{
protected:
        std::shared_ptr<MockLinux> mock_linux_;
        std::shared_ptr<MockLog> mock_log_;
        MemBuffer request_;
        MessageType request_type_;
        MemBuffer response_;
                
	registryproxy_tests()
                : mock_linux_(),
                  mock_log_(),
                  request_(),
                  request_type_(kTextMessage),
                  response_() {
                mock_linux_ = std::make_shared<MockLinux>();
                mock_log_ = std::make_shared<MockLog>();
        }

	~registryproxy_tests() override = default;

	void SetUp() override {
        }
        
	void TearDown() override {
	}

public:
        
        void copy_request(MemBuffer& message, MessageType type) {
                request_.clear();
                request_.append(message);
                request_type_ = type;
        }
        
        void copy_response(MemBuffer& message, double) {
                message.clear();
                message.append(response_);
        }
};

TEST_F(registryproxy_tests, constructor_doesnt_throw)
{
        // Arrange
        std::unique_ptr<MockWebSocket> mock_websocket = std::make_unique<MockWebSocket>();
        EXPECT_CALL(*mock_websocket, close(_))
                .Times(1);
        
        std::unique_ptr<IWebSocket> websocket = std::move(mock_websocket);
        
        // Act
        // Assert
        ASSERT_NO_THROW(RegistryProxy registry(websocket, mock_linux_, mock_log_));
}

TEST_F(registryproxy_tests, constructor_remove_successfully)
{
        // Arrange
        response_.clear();
        response_.append("{\"success\": true}");
        
        std::unique_ptr<MockWebSocket> mock_websocket = std::make_unique<MockWebSocket>();
        
        EXPECT_CALL(*mock_websocket, send(_,_))
                .WillRepeatedly(DoAll(Invoke(this, &registryproxy_tests::copy_request),
                                      Return(true)));
        
        EXPECT_CALL(*mock_websocket, recv(_,_))
                .WillRepeatedly(DoAll(Invoke(this, &registryproxy_tests::copy_response),
                                      Return(rcom::kRecvText)));
        
        EXPECT_CALL(*mock_websocket, close(_))
                .Times(1);
        
        std::unique_ptr<IWebSocket> websocket = std::move(mock_websocket);
        
        // Act
        RegistryProxy registry(websocket, mock_linux_, mock_log_);
        
        // Assert
        ASSERT_NO_THROW(registry.remove("dummy"));
}

TEST_F(registryproxy_tests, constructor_remove_throws_error_on_false_response)
{
        // Arrange
        response_.clear();
        response_.append("{\"success\": false}");
        
        std::unique_ptr<MockWebSocket> mock_websocket = std::make_unique<MockWebSocket>();
        
        EXPECT_CALL(*mock_websocket, send(_,_))
                .WillRepeatedly(DoAll(Invoke(this, &registryproxy_tests::copy_request),
                                      Return(true)));
        
        EXPECT_CALL(*mock_websocket, recv(_,_))
                .WillRepeatedly(DoAll(Invoke(this, &registryproxy_tests::copy_response),
                                      Return(rcom::kRecvText)));
        
        EXPECT_CALL(*mock_websocket, close(_))
                .Times(1);

        EXPECT_CALL(*mock_log_, warn(_))
                .Times(1);
        
        std::unique_ptr<IWebSocket> websocket = std::move(mock_websocket);
        
        // Act
        RegistryProxy registry(websocket, mock_linux_, mock_log_);
        
        // Assert
        ASSERT_THROW(registry.remove("dummy"), std::runtime_error);
}

TEST_F(registryproxy_tests, constructor_remove_throws_error_on_ill_response)
{
        // Arrange
        response_.clear();
        response_.append("{\"dummy\": true}");
        
        std::unique_ptr<MockWebSocket> mock_websocket = std::make_unique<MockWebSocket>();
        
        EXPECT_CALL(*mock_websocket, send(_,_))
                .WillRepeatedly(DoAll(Invoke(this, &registryproxy_tests::copy_request),
                                      Return(true)));
        
        EXPECT_CALL(*mock_websocket, recv(_,_))
                .WillRepeatedly(DoAll(Invoke(this, &registryproxy_tests::copy_response),
                                      Return(rcom::kRecvText)));
        
        EXPECT_CALL(*mock_websocket, close(_))
                .Times(1);

        EXPECT_CALL(*mock_log_, error(_))
                .Times(1);

        std::unique_ptr<IWebSocket> websocket = std::move(mock_websocket);
        
        // Act
        RegistryProxy registry(websocket, mock_linux_, mock_log_);
        
        // Assert
        ASSERT_THROW(registry.remove("dummy"), std::runtime_error);
}


