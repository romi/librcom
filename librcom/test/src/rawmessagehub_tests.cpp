#include <string>
#include "gtest/gtest.h"

// Need to include r.h here to make sure when we mock them below they
// are mocked in the link with the tests.

#include "RawMessageHub.h"
#include "MessageListener.mock.h"
#include "WebSocketServerFactory.mock.h"

using namespace rcom;

class rawmessagehub_tests : public ::testing::Test
{
protected:
	rawmessagehub_tests() = default;

	~rawmessagehub_tests() override = default;

	void SetUp() override {
        }
        
	void TearDown() override {
	}
};

TEST_F(rawmessagehub_tests, constructor_invalid_topic_throws)
{
        // Arrange
        auto mockMessageListener = std::make_shared<MockMessageListener>();
        auto mockWebServerSocketFactory = std::make_shared<MockWebSocketServerFactory>();
        std::string topic(" ");

        // Act
        //Assert
        ASSERT_THROW(RawMessageHub rawMessageHub(topic, mockMessageListener, mockWebServerSocketFactory), std::invalid_argument);
}


