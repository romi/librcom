#include <string>
#include "gtest/gtest.h"

// Need to include r.h here to make sure when we mock them below they
// are mocked in the link with the tests.
#include "r.h"
#include "Address.h"

using namespace rcom;

class address_tests : public ::testing::Test
{
protected:
	address_tests() = default;

	~address_tests() override = default;

	void SetUp() override {
        }
        
	void TearDown() override {
	}
};

TEST_F(address_tests, address_constructor_initializes_correctly_1)
{
        // Arrange
        const char *ip = "127.0.0.1";
        uint16_t port = 123;
        
        // Act
        Address address(ip, port);

        //Assert
        std::string actual_ip;
        ASSERT_TRUE(address.is_set());
        ASSERT_STREQ(address.ip(actual_ip).c_str(), ip);
        ASSERT_EQ(address.port(), port);
}

TEST_F(address_tests, address_constructor_initializes_correctly_2)
{
        // Arrange
        const char *ip = "127.0.0.1";
        uint16_t port = 123;
        
        std::string arg = ip;
        arg += ":";
        arg += std::to_string(port);
        
        // Act
        Address address(arg);

        //Assert
        std::string actual_ip;
        ASSERT_TRUE(address.is_set());
        ASSERT_STREQ(address.ip(actual_ip).c_str(), ip);
        ASSERT_EQ(address.port(), port);
}

TEST_F(address_tests, address_constructor_with_no_args_is_not_set)
{
        // Arrange

        // Act
        Address address;

        //Assert
        ASSERT_FALSE(address.is_set());
}

TEST_F(address_tests, address_with_null_throws_exception_1)
{
        // Arrange
        uint16_t port = 123;

        try {
                // Act
                Address address(nullptr, port);
                FAIL() << "Expected std::runtime_error";
                
        } catch(std::runtime_error const & err) {
                // OK
                
        } catch(...) {
                FAIL() << "Expected std::runtime_error";
        }
}

TEST_F(address_tests, address_with_null_throws_exception_2)
{
        // Arrange

        try {
                // Act
                Address address(nullptr);
                FAIL() << "Expected exception";
                
        } catch(...) {
                // OK
        }
}

TEST_F(address_tests, address_too_short_throws_exception)
{
        // Arrange
        const char *ip = "127.0.0.";
        uint16_t port = 123;

        try {
                // Act
                Address address(ip, port);
                FAIL() << "Expected std::runtime_error";
                
        } catch(std::runtime_error const & err) {
                // OK
                
        } catch(...) {
                FAIL() << "Expected std::runtime_error";
        }
}

TEST_F(address_tests, address_too_long_throws_exception_1)
{
        // Arrange
        const char *ip = "127.0.0.0.1";
        uint16_t port = 123;

        try {
                // Act
                Address address(ip, port);
                FAIL() << "Expected std::runtime_error";
                
        } catch(std::runtime_error const & err) {
                // OK
                
        } catch(...) {
                FAIL() << "Expected std::runtime_error";
        }
}

TEST_F(address_tests, address_too_long_throws_exception_2)
{
        // Arrange
        const char *ip = "1270.0.0.1";
        uint16_t port = 123;

        try {
                // Act
                Address address(ip, port);
                FAIL() << "Expected std::runtime_error";
                
        } catch(std::runtime_error const & err) {
                // OK
                
        } catch(...) {
                FAIL() << "Expected std::runtime_error";
        }
}

TEST_F(address_tests, address_with_invalid_port_throws_exception_1)
{
        // Arrange
        const char *bad_address = "127.0.0.1:abc";

        try {
                // Act
                Address address(bad_address);
                FAIL() << "Expected std::runtime_error";
                
        } catch(std::runtime_error const & err) {
                // OK
                
        } catch(...) {
                FAIL() << "Expected std::runtime_error";
        }
}

TEST_F(address_tests, address_with_invalid_port_throws_exception_2)
{
        // Arrange
        const char *bad_address = "127.0.0.1: ";

        try {
                // Act
                Address address(bad_address);
                FAIL() << "Expected std::runtime_error";
                
        } catch(std::runtime_error const & err) {
                // OK
                
        } catch(...) {
                FAIL() << "Expected std::runtime_error";
        }
}

TEST_F(address_tests, address_with_spaces_parses_ok)
{
        // Arrange
        const char *ip = "127.0.0.1";
        uint16_t port = 123;
        const char *s = "127.0.0.1  :  123";

        try {
                // Act
                Address address(s);
                
                //Assert
                std::string actual_ip;
                ASSERT_TRUE(address.is_set());
                ASSERT_STREQ(address.ip(actual_ip).c_str(), ip);
                ASSERT_EQ(address.port(), port);
                
        } catch(...) {
                FAIL() << "Expected success";
        }
}

TEST_F(address_tests, set_with_two_args_initializes_the_address)
{
        // Arrange
        const char *ip = "127.0.0.1";
        uint16_t port = 123;

        // Act
        Address address;
        address.set(ip, port);
        
        //Assert
        std::string actual_ip;
        ASSERT_TRUE(address.is_set());
        ASSERT_STREQ(address.ip(actual_ip).c_str(), ip);
        ASSERT_EQ(address.port(), port);
}

TEST_F(address_tests, set_with_one_string_initializes_the_address)
{
        // Arrange
        const char *ip = "127.0.0.1";
        uint16_t port = 123;
        std::string address_string = ip;
        address_string += ":";
        address_string += std::to_string(port);

        // Act
        Address address;
        address.set(address_string);
        
        //Assert
        std::string actual_address;
        address.tostring(actual_address);
        ASSERT_TRUE(address.is_set());
        ASSERT_STREQ(actual_address.c_str(), address_string.c_str());
}

TEST_F(address_tests, set_with_address_initializes_the_address)
{
        // Arrange
        const char *ip = "127.0.0.1";
        uint16_t port = 123;
        Address address(ip, port);
        std::string expected_address;
        address.tostring(expected_address);

        // Act
        Address clone;
        clone.set(address);
        
        //Assert
        std::string actual_adress;
        clone.tostring(actual_adress);
        ASSERT_TRUE(clone.is_set());
        ASSERT_STREQ(actual_adress.c_str(), expected_address.c_str());
}

TEST_F(address_tests, set_overwrites_the_address)
{
        // Arrange
        const char *ip = "127.0.0.1";
        uint16_t port = 123;
        const char *expected_ip = "127.0.0.2";
        uint16_t expected_port = 124;

        // Act
        Address address(ip, port);
        address.set(expected_ip, expected_port);
        
        //Assert
        std::string actual_ip;
        ASSERT_TRUE(address.is_set());
        ASSERT_STREQ(address.ip(actual_ip).c_str(), expected_ip);
        ASSERT_EQ(address.port(), expected_port);
}

TEST_F(address_tests, constructier_without_args_leaves_address_unset)
{
        // Arrange

        // Act
        Address address;
        
        //Assert
        ASSERT_FALSE(address.is_set());
}

TEST_F(address_tests, constructier_with_args_makes_address_set)
{
        // Arrange
        const char *ip = "127.0.0.1";
        uint16_t port = 123;

        // Act
        Address address(ip, port);
        
        //Assert
        ASSERT_TRUE(address.is_set());
}

TEST_F(address_tests, tostring_returns_expected_value)
{
        // Arrange
        const char *ip = "127.0.0.1";
        uint16_t port = 123;
        std::string expected = ip;
        expected += ":";
        expected += std::to_string(port);
        
        // Act
        Address address(ip, port);
        std::string actual;
        address.tostring(actual);
        
        //Assert
        ASSERT_STREQ(actual.c_str(), expected.c_str());
}
