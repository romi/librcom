#include <string>
#include "gtest/gtest.h"

#include "rcom/Registry.h"

using namespace rcom;

class registry_tests : public ::testing::Test
{
protected:
	registry_tests() = default;

	~registry_tests() override = default;

	void SetUp() override {
        }
        
	void TearDown() override {
	}
};

TEST_F(registry_tests, constructor_returns_empty_register_duh)
{
        // Arrange
        Address address;
        
        // Act
        Registry registry;

        // Assert
        ASSERT_FALSE(registry.get("bugs", address, 10.0)); // and we're done
}

TEST_F(registry_tests, get_returns_expected_values)
{
        // Arrange
        std::string topic = "test";
        Address expected_address("127.0.0.1:123");
        Address actual_address;
        Registry registry;
        
        // Actc
        registry.set(topic, expected_address);
        bool get_succeeded  = registry.get(topic, actual_address, 10.0);
        
        // Assert
        std::string s;
        std::string t;
        ASSERT_TRUE(get_succeeded);
        ASSERT_STREQ(actual_address.tostring(s).c_str(),
                     expected_address.tostring(t).c_str());
}

TEST_F(registry_tests, remove_removes_entry)
{
        // Arrange
        std::string topic = "test";
        Address address("127.0.0.1:123");
        Address actual_address;
        Registry registry;
        
        // Actc
        registry.set(topic, address);
        registry.remove(topic);
        bool has = registry.get(topic, actual_address, 10.0);
        
        // Assert
        ASSERT_FALSE(has);
}

TEST_F(registry_tests, set_removes_old_value)
{
        // Arrange
        std::string topic = "test";
        Address first_address("127.0.0.1:1");
        Address second_address("127.0.0.2:2");
        Address actual_address;        
        Registry registry;
        
        // Act
        registry.set(topic, first_address);
        registry.set(topic, second_address);
        bool get_succeeded  = registry.get(topic, actual_address, 10.0);
        
        // Assert
        std::string s;
        std::string t;
        ASSERT_TRUE(get_succeeded);
        ASSERT_STREQ(actual_address.tostring(s).c_str(),
                     second_address.tostring(t).c_str());
}


