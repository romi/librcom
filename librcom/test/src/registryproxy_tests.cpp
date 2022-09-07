#include <string>
#include "gtest/gtest.h"

#include "rcom/RegistryProxy.h"

#include "WebSocket.mock.h"
#include "Linux.mock.h"
#include "Log.mock.h"

using namespace rcom;

using ::testing::_;
using ::testing::Return;
using ::testing::AtLeast;

class registryproxy_tests : public ::testing::Test
{
protected:
        std::shared_ptr<MockLinux> mock_linux_;
        std::shared_ptr<MockLog> mock_log_;
        MemBuffer request_;
        MessageType request_type_;
        MemBuffer response_;
        int clock_gettime_counter_;
        int clock_gettime_increment_;
        double timeout_;
        
	registryproxy_tests()
                : mock_linux_(),
                  mock_log_(),
                  request_(),
                  request_type_(kTextMessage),
                  response_(),
                  clock_gettime_counter_(0),
                  clock_gettime_increment_(0),
                  timeout_(10.0)
                {
                mock_linux_ = std::make_shared<MockLinux>();
                mock_log_ = std::make_shared<MockLog>();
        }

	~registryproxy_tests() override = default;

	void SetUp() override {
                clock_gettime_counter_ = 0;
                clock_gettime_increment_ = 0;
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
        
        int clock_gettime(clockid_t, struct timespec *tp) 
        {
                tp->tv_sec = clock_gettime_counter_ * clock_gettime_increment_;
                tp->tv_nsec = 0;
                clock_gettime_counter_++;
                return 0;
        }
        
        int clock_nanosleep(clockid_t, int, const struct timespec *,
                            struct timespec *remain) 
        {
                remain->tv_sec = 0;
                remain->tv_nsec = 0;
                return 0;
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

TEST_F(registryproxy_tests, remove_successfully)
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

TEST_F(registryproxy_tests, remove_throws_error_when_send_fails)
{
        // Arrange
        std::unique_ptr<MockWebSocket> mock_websocket = std::make_unique<MockWebSocket>();
        
        EXPECT_CALL(*mock_websocket, send(_,_))
                .WillOnce(Return(false));
        
        EXPECT_CALL(*mock_websocket, close(_))
                .Times(1);

        EXPECT_CALL(*mock_log_, error(_))
                .Times(AtLeast(1));
        
        std::unique_ptr<IWebSocket> websocket = std::move(mock_websocket);
        
        // Act
        RegistryProxy registry(websocket, mock_linux_, mock_log_);
        
        // Assert
        ASSERT_THROW(registry.remove("dummy"), std::runtime_error);
}

TEST_F(registryproxy_tests, remove_throws_error_when_recv_fails)
{
        // Arrange
        std::unique_ptr<MockWebSocket> mock_websocket = std::make_unique<MockWebSocket>();
        
        EXPECT_CALL(*mock_websocket, send(_,_))
                .WillRepeatedly(DoAll(Invoke(this, &registryproxy_tests::copy_request),
                                      Return(true)));
        
        EXPECT_CALL(*mock_websocket, recv(_,_))
                .WillRepeatedly(Return(rcom::kRecvError));
        
        EXPECT_CALL(*mock_websocket, close(_))
                .Times(1);

        EXPECT_CALL(*mock_log_, error(_))
                .Times(AtLeast(1));
        
        std::unique_ptr<IWebSocket> websocket = std::move(mock_websocket);
        
        // Act
        RegistryProxy registry(websocket, mock_linux_, mock_log_);
        
        // Assert
        ASSERT_THROW(registry.remove("dummy"), std::runtime_error);
}

TEST_F(registryproxy_tests, remove_throws_error_when_recv_times_out)
{
        // Arrange
        std::unique_ptr<MockWebSocket> mock_websocket = std::make_unique<MockWebSocket>();
        
        EXPECT_CALL(*mock_websocket, send(_,_))
                .WillRepeatedly(DoAll(Invoke(this, &registryproxy_tests::copy_request),
                                      Return(true)));
        
        EXPECT_CALL(*mock_websocket, recv(_,_))
                .WillRepeatedly(Return(rcom::kRecvTimeOut));
        
        EXPECT_CALL(*mock_websocket, close(_))
                .Times(1);

        EXPECT_CALL(*mock_log_, error(_))
                .Times(AtLeast(1));
        
        std::unique_ptr<IWebSocket> websocket = std::move(mock_websocket);
        
        // Act
        RegistryProxy registry(websocket, mock_linux_, mock_log_);
        
        // Assert
        ASSERT_THROW(registry.remove("dummy"), std::runtime_error);
}

TEST_F(registryproxy_tests, remove_throws_error_on_ill_formed_response)
{
        // Arrange
        response_.clear();
        response_.append("{\"success\": true");
        
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
                .Times(AtLeast(1));

        std::unique_ptr<IWebSocket> websocket = std::move(mock_websocket);
        
        // Act
        RegistryProxy registry(websocket, mock_linux_, mock_log_);
        
        // Assert
        ASSERT_THROW(registry.remove("dummy"), std::runtime_error);
}

TEST_F(registryproxy_tests, remove_throws_error_on_bad_response)
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
                .Times(AtLeast(1));

        std::unique_ptr<IWebSocket> websocket = std::move(mock_websocket);
        
        // Act
        RegistryProxy registry(websocket, mock_linux_, mock_log_);
        
        // Assert
        ASSERT_THROW(registry.remove("dummy"), std::runtime_error);
}

TEST_F(registryproxy_tests, remove_throws_error_on_false_response)
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

        EXPECT_CALL(*mock_log_, error(_))
                .Times(AtLeast(1));

        EXPECT_CALL(*mock_log_, warn(_))
                .Times(AtLeast(0));
        
        std::unique_ptr<IWebSocket> websocket = std::move(mock_websocket);
        
        // Act
        RegistryProxy registry(websocket, mock_linux_, mock_log_);
        
        // Assert
        ASSERT_THROW(registry.remove("dummy"), std::runtime_error);
}

TEST_F(registryproxy_tests, get_successfully)
{
        // Arrange
        std::string expected = "127.0.0.1:1000";
        response_.clear();
        response_.append("{\"success\": true, \"address\": \"127.0.0.1:1000\"}");
        
        std::unique_ptr<MockWebSocket> mock_websocket = std::make_unique<MockWebSocket>();
        
        EXPECT_CALL(*mock_websocket, send(_,_))
                .WillRepeatedly(DoAll(Invoke(this, &registryproxy_tests::copy_request),
                                      Return(true)));
        
        EXPECT_CALL(*mock_websocket, recv(_,_))
                .WillRepeatedly(DoAll(Invoke(this, &registryproxy_tests::copy_response),
                                      Return(rcom::kRecvText)));
        
        EXPECT_CALL(*mock_websocket, close(_))
                .Times(1);

        EXPECT_CALL(*mock_linux_, clock_gettime(_,_))
                .WillRepeatedly(DoAll(Invoke(this, &registryproxy_tests::clock_gettime),
                                      Return(0)));

        std::unique_ptr<IWebSocket> websocket = std::move(mock_websocket);
        RegistryProxy registry(websocket, mock_linux_, mock_log_);

        // Act
        rcom::Address address;
        registry.get("dummy", address, timeout_);
                        
        // Assert
        std::string result;
        address.tostring(result);
        ASSERT_EQ(result, expected);
}

TEST_F(registryproxy_tests, get_throws_when_send_fails)
{
        // Arrange
        std::unique_ptr<MockWebSocket> mock_websocket = std::make_unique<MockWebSocket>();
        
        EXPECT_CALL(*mock_websocket, send(_,_))
                .WillRepeatedly(Return(false));
        
        EXPECT_CALL(*mock_websocket, close(_))
                .Times(1);
                
        EXPECT_CALL(*mock_linux_, clock_gettime(_,_))
                .WillRepeatedly(DoAll(Invoke(this, &registryproxy_tests::clock_gettime),
                                      Return(0)));
                                
        EXPECT_CALL(*mock_linux_, clock_nanosleep(_,_,_,_))
                .WillRepeatedly(DoAll(Invoke(this, &registryproxy_tests::clock_nanosleep),
                                      Return(0)));
        
        EXPECT_CALL(*mock_log_, error(_))
                .Times(AtLeast(1));

        std::unique_ptr<IWebSocket> websocket = std::move(mock_websocket);
        RegistryProxy registry(websocket, mock_linux_, mock_log_);

        // Act
        // Assert
        rcom::Address address;
        ASSERT_THROW(registry.get("dummy", address, timeout_), std::runtime_error);
}

TEST_F(registryproxy_tests, get_throws_when_recv_return_err)
{
        // Arrange
        std::unique_ptr<MockWebSocket> mock_websocket = std::make_unique<MockWebSocket>();
        
        EXPECT_CALL(*mock_websocket, send(_,_))
                .WillRepeatedly(DoAll(Invoke(this, &registryproxy_tests::copy_request),
                                      Return(true)));
        
        EXPECT_CALL(*mock_websocket, recv(_,_))
                .WillRepeatedly(Return(rcom::kRecvError));
        
        EXPECT_CALL(*mock_websocket, close(_))
                .Times(1);
        
        EXPECT_CALL(*mock_linux_, clock_gettime(_,_))
                .WillRepeatedly(DoAll(Invoke(this, &registryproxy_tests::clock_gettime),
                                      Return(0)));
        
        EXPECT_CALL(*mock_log_, error(_))
                .Times(AtLeast(1));
        
        std::unique_ptr<IWebSocket> websocket = std::move(mock_websocket);
        RegistryProxy registry(websocket, mock_linux_, mock_log_);

        // Act
        // Assert
        rcom::Address address;
        ASSERT_THROW(registry.get("dummy", address, timeout_), std::runtime_error);
}

TEST_F(registryproxy_tests, get_throws_when_recv_times_out)
{
        // Arrange
        std::unique_ptr<MockWebSocket> mock_websocket = std::make_unique<MockWebSocket>();
        
        EXPECT_CALL(*mock_websocket, send(_,_))
                .WillRepeatedly(DoAll(Invoke(this, &registryproxy_tests::copy_request),
                                      Return(true)));
        
        EXPECT_CALL(*mock_websocket, recv(_,_))
                .WillRepeatedly(Return(rcom::kRecvTimeOut));
        
        EXPECT_CALL(*mock_websocket, close(_))
                .Times(1);
        
        EXPECT_CALL(*mock_linux_, clock_gettime(_,_))
                .WillRepeatedly(DoAll(Invoke(this, &registryproxy_tests::clock_gettime),
                                      Return(0)));
        
        EXPECT_CALL(*mock_log_, error(_))
                .Times(AtLeast(1));
        
        std::unique_ptr<IWebSocket> websocket = std::move(mock_websocket);
        RegistryProxy registry(websocket, mock_linux_, mock_log_);

        // Act
        // Assert
        rcom::Address address;
        ASSERT_THROW(registry.get("dummy", address, timeout_), std::runtime_error);
}

TEST_F(registryproxy_tests, get_returns_false_when_no_address_found)
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

        clock_gettime_increment_ = 1;

        EXPECT_CALL(*mock_linux_, clock_gettime(_,_))
                .WillRepeatedly(DoAll(Invoke(this, &registryproxy_tests::clock_gettime),
                                      Return(0)));

        EXPECT_CALL(*mock_linux_, clock_nanosleep(_,_,_,_))
                .WillRepeatedly(DoAll(Invoke(this, &registryproxy_tests::clock_nanosleep),
                                      Return(0)));
        
        EXPECT_CALL(*mock_log_, warn(_))
                .Times(AtLeast(1));
        
        std::unique_ptr<IWebSocket> websocket = std::move(mock_websocket);
        RegistryProxy registry(websocket, mock_linux_, mock_log_);

        // Act
        rcom::Address address;
        bool success = registry.get("dummy", address, timeout_);
        
        // Assert
        ASSERT_EQ(success, false);
}

TEST_F(registryproxy_tests, get_throws_when_invalid_response_1)
{
        // Arrange
        response_.clear();
        response_.append("{\"success\": false");
        std::unique_ptr<MockWebSocket> mock_websocket = std::make_unique<MockWebSocket>();
        
        EXPECT_CALL(*mock_websocket, send(_,_))
                .WillRepeatedly(DoAll(Invoke(this, &registryproxy_tests::copy_request),
                                      Return(true)));
        
        EXPECT_CALL(*mock_websocket, recv(_,_))
                .WillRepeatedly(DoAll(Invoke(this, &registryproxy_tests::copy_response),
                                      Return(rcom::kRecvText)));
        
        EXPECT_CALL(*mock_websocket, close(_))
                .Times(1);

        clock_gettime_increment_ = 1;

        EXPECT_CALL(*mock_linux_, clock_gettime(_,_))
                .WillRepeatedly(DoAll(Invoke(this, &registryproxy_tests::clock_gettime),
                                      Return(0)));

        EXPECT_CALL(*mock_linux_, clock_nanosleep(_,_,_,_))
                .WillRepeatedly(DoAll(Invoke(this, &registryproxy_tests::clock_nanosleep),
                                      Return(0)));
        
        EXPECT_CALL(*mock_log_, error(_))
                .Times(AtLeast(1));
        
        std::unique_ptr<IWebSocket> websocket = std::move(mock_websocket);
        RegistryProxy registry(websocket, mock_linux_, mock_log_);

        // Act
        rcom::Address address;
        ASSERT_THROW(registry.get("dummy", address, timeout_), std::runtime_error);
}

TEST_F(registryproxy_tests, get_throws_when_invalid_response_2)
{
        // Arrange
        response_.clear();
        response_.append("{\"success\": true, \"addr\": \"127.0.0.1:1000\"}");
        std::unique_ptr<MockWebSocket> mock_websocket = std::make_unique<MockWebSocket>();
        
        EXPECT_CALL(*mock_websocket, send(_,_))
                .WillRepeatedly(DoAll(Invoke(this, &registryproxy_tests::copy_request),
                                      Return(true)));
        
        EXPECT_CALL(*mock_websocket, recv(_,_))
                .WillRepeatedly(DoAll(Invoke(this, &registryproxy_tests::copy_response),
                                      Return(rcom::kRecvText)));
        
        EXPECT_CALL(*mock_websocket, close(_))
                .Times(1);

        clock_gettime_increment_ = 1;

        EXPECT_CALL(*mock_linux_, clock_gettime(_,_))
                .WillRepeatedly(DoAll(Invoke(this, &registryproxy_tests::clock_gettime),
                                      Return(0)));

        EXPECT_CALL(*mock_linux_, clock_nanosleep(_,_,_,_))
                .WillRepeatedly(DoAll(Invoke(this, &registryproxy_tests::clock_nanosleep),
                                      Return(0)));
        
        EXPECT_CALL(*mock_log_, error(_))
                .Times(AtLeast(1));
        
        std::unique_ptr<IWebSocket> websocket = std::move(mock_websocket);
        RegistryProxy registry(websocket, mock_linux_, mock_log_);

        // Act
        rcom::Address address;
        ASSERT_THROW(registry.get("dummy", address, timeout_), std::runtime_error);
}

TEST_F(registryproxy_tests, get_throws_when_invalid_response_3)
{
        // Arrange
        response_.clear();
        response_.append("{\"success\": true, \"address\": \"bad-address\"}");
        std::unique_ptr<MockWebSocket> mock_websocket = std::make_unique<MockWebSocket>();
        
        EXPECT_CALL(*mock_websocket, send(_,_))
                .WillRepeatedly(DoAll(Invoke(this, &registryproxy_tests::copy_request),
                                      Return(true)));
        
        EXPECT_CALL(*mock_websocket, recv(_,_))
                .WillRepeatedly(DoAll(Invoke(this, &registryproxy_tests::copy_response),
                                      Return(rcom::kRecvText)));
        
        EXPECT_CALL(*mock_websocket, close(_))
                .Times(1);

        clock_gettime_increment_ = 1;

        EXPECT_CALL(*mock_linux_, clock_gettime(_,_))
                .WillRepeatedly(DoAll(Invoke(this, &registryproxy_tests::clock_gettime),
                                      Return(0)));

        EXPECT_CALL(*mock_linux_, clock_nanosleep(_,_,_,_))
                .WillRepeatedly(DoAll(Invoke(this, &registryproxy_tests::clock_nanosleep),
                                      Return(0)));
        
        EXPECT_CALL(*mock_log_, error(_))
                .Times(AtLeast(0));
        
        std::unique_ptr<IWebSocket> websocket = std::move(mock_websocket);
        RegistryProxy registry(websocket, mock_linux_, mock_log_);

        // Act
        rcom::Address address;
        ASSERT_THROW(registry.get("dummy", address, timeout_), std::runtime_error);
}

TEST_F(registryproxy_tests, get_throws_when_invalid_response_4)
{
        // Arrange
        response_.clear();
        response_.append("{\"no-success\": false}");
        std::unique_ptr<MockWebSocket> mock_websocket = std::make_unique<MockWebSocket>();
        
        EXPECT_CALL(*mock_websocket, send(_,_))
                .WillRepeatedly(DoAll(Invoke(this, &registryproxy_tests::copy_request),
                                      Return(true)));
        
        EXPECT_CALL(*mock_websocket, recv(_,_))
                .WillRepeatedly(DoAll(Invoke(this, &registryproxy_tests::copy_response),
                                      Return(rcom::kRecvText)));
        
        EXPECT_CALL(*mock_websocket, close(_))
                .Times(1);

        clock_gettime_increment_ = 1;

        EXPECT_CALL(*mock_linux_, clock_gettime(_,_))
                .WillRepeatedly(DoAll(Invoke(this, &registryproxy_tests::clock_gettime),
                                      Return(0)));

        EXPECT_CALL(*mock_linux_, clock_nanosleep(_,_,_,_))
                .WillRepeatedly(DoAll(Invoke(this, &registryproxy_tests::clock_nanosleep),
                                      Return(0)));
        
        EXPECT_CALL(*mock_log_, error(_))
                .Times(AtLeast(1));
        
        std::unique_ptr<IWebSocket> websocket = std::move(mock_websocket);
        RegistryProxy registry(websocket, mock_linux_, mock_log_);

        // Act
        rcom::Address address;
        ASSERT_THROW(registry.get("dummy", address, timeout_), std::runtime_error);
}


