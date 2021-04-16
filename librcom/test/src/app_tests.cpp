#include <string>
#include "gtest/gtest.h"

// Need to include r.h here to make sure when we mock them below they
// are mocked in the link with the tests.
#include "r.h"
#include "App.h"
#include "util.h"

using namespace rcom;

class app_tests : public ::testing::Test
{
protected:
	app_tests() = default;

	~app_tests() override = default;

	void SetUp() override {
        }
        
	void TearDown() override {
	}
};

TEST_F(app_tests, app_has_valid_id_after_empty_constructor)
{
        // Arrange
        
        // Act
        App app;

        //Assert
        ASSERT_TRUE(is_valid_uuid(app.id().c_str()));
}

TEST_F(app_tests, app_has_expected_id_after_constructor_with_args)
{
        // Arrange
        const char *id = "12345678-1234-1234-1234-123456789012";
        
        // Act
        App app(id);

        //Assert
        ASSERT_STREQ(app.id().c_str(), id);
}

TEST_F(app_tests, constructor_with_null_id_throws_exception)
{
        try {
                // Arrange
                const char *id = nullptr;
                
                // Act
                App app(id);
                FAIL() << "Expected std::runtime_error";

       } catch(std::runtime_error const & err) {
                // OK
                
        } catch(...) {
                FAIL() << "Expected std::runtime_error";
        }
}

TEST_F(app_tests, constructor_with_invalid_id_throws_exception)
{
        try {
                // Arrange
                const char *id = "12345678-1234-1234-1234";
                
                // Act
                App app(id);
                FAIL() << "Expected std::runtime_error";

       } catch(std::runtime_error const & err) {
                // OK
                
        } catch(...) {
                FAIL() << "Expected std::runtime_error";
        }
}

TEST_F(app_tests, app_has_expected_id_after_set_id)
{
        // Arrange
        const char *id = "12345678-1234-1234-1234-123456789012";
        
        // Act
        App app;
        app.set_id(id);

        //Assert
        ASSERT_STREQ(app.id().c_str(), id);
}
