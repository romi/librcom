#include <string>
#include "gtest/gtest.h"

#include "rcom/RegistryProxy.h"

#include "WebSocket.mock.h"
#include "Linux.mock.h"
#include "Log.mock.h"

using namespace rcom;

using ::testing::_;

class registryproxy_tests : public ::testing::Test
{
protected:
        std::shared_ptr<MockLinux> mock_linux_;
        std::shared_ptr<MockLog> mock_log_;

	registryproxy_tests()
                : mock_linux_(),
                  mock_log_() {
                mock_linux_ = std::make_shared<MockLinux>();
                mock_log_ = std::make_shared<MockLog>();
        }

	~registryproxy_tests() override = default;

	void SetUp() override {
        }
        
	void TearDown() override {
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


