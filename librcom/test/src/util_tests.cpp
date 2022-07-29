#include <string>
#include "gtest/gtest.h"

// Need to include r.h here to make sure when we mock them below they
// are mocked in the link with the tests.
#include "util.h"

using namespace rcom;

class util_tests : public ::testing::Test
{
protected:
	util_tests() = default;

	~util_tests() override = default;

	void SetUp() override {
        }
        
	void TearDown() override {
	}
};

TEST_F(util_tests, sha1_returns_expected_value)
{
        // Arrange
        std::string input = "abc";
        unsigned char digest[20];
        unsigned char expected_digest[] = { 0xA9, 0x99, 0x3E, 0x36, 0x47,
                                            0x06, 0x81, 0x6A, 0xBA, 0x3E,
                                            0x25, 0x71, 0x78, 0x50, 0xC2,
                                            0x6C, 0x9C, 0xD0, 0xD8, 0x9D};
        
        // Act
        SHA1(input, digest);

        //Assert
        ASSERT_TRUE(memcmp(digest, expected_digest, 20) == 0);
}

TEST_F(util_tests, is_valid_topic_returns_true)
{
        // Arrange
        const char *topic = "topic-with-hyphens";
        
        // Act
        bool valid = is_valid_topic(topic);

        //Assert
        ASSERT_TRUE(valid);
}

TEST_F(util_tests, is_valid_topic_returns_false_on_too_short_name)
{
        // Arrange
        const char *topic = "t";
        
        // Act
        bool valid = is_valid_topic(topic);

        //Assert
        ASSERT_FALSE(valid);
}

TEST_F(util_tests, is_valid_topic_returns_false_on_too_long_name)
{
        // Arrange
        const char *topic = ("once-upon-a-time-in-a-land-far-away-"
                             "something-magic-happened-but-no-one-remembered");
        
        // Act
        bool valid = is_valid_topic(topic);

        //Assert
        ASSERT_FALSE(valid);
}

TEST_F(util_tests, is_valid_topic_returns_false_on_name_with_invalid_char)
{
        // Arrange
        const char *topic = "once-upon-a-f#ck!n$-time";
        
        // Act
        bool valid = is_valid_topic(topic);

        //Assert
        ASSERT_FALSE(valid);
}

TEST_F(util_tests, is_valid_topic_returns_false_on_name_starting_with_a_digit)
{
        // Arrange
        const char *topic = "1st-topic";
        
        // Act
        bool valid = is_valid_topic(topic);

        //Assert
        ASSERT_FALSE(valid);
}

TEST_F(util_tests, base64_returns_expected_value_1)
{
        // Arrange
        const char *input = "a";
        const char *expected = "YQ==";
        std::string output;
        
        // Act
        encode_base64((const unsigned char *) input, strlen(input), output);

        //Assert
        ASSERT_STREQ(output.c_str(), expected);
}

TEST_F(util_tests, base64_returns_expected_value_2)
{
        // Arrange
        const char *input = "ab";
        const char *expected = "YWI=";
        std::string output;
        
        // Act
        encode_base64((const unsigned char *) input, strlen(input), output);

        //Assert
        ASSERT_STREQ(output.c_str(), expected);
}

TEST_F(util_tests, base64_returns_expected_value_3)
{
        // Arrange
        const char *input = "abc";
        const char *expected = "YWJj";
        std::string output;
        
        // Act
        encode_base64((const unsigned char *) input, strlen(input), output);

        //Assert
        ASSERT_STREQ(output.c_str(), expected);
}

TEST_F(util_tests, base64_returns_expected_value_4)
{
        // Arrange
        const char *input = ("A distributed system is one in which "
                             "the failure of a computer you didn't even know existed "
                             "can render your own computer unusable. (Leslie Lamport)");
        
        const char *expected = ("QSBkaXN0cmlidXRlZCBzeXN0ZW0gaXMgb25lIG"
                                "luIHdoaWNoIHRoZSBmYWlsdXJlIG9mIGEgY29t"
                                "cHV0ZXIgeW91IGRpZG4ndCBldmVuIGtub3cgZX"
                                "hpc3RlZCBjYW4gcmVuZGVyIHlvdXIgb3duIGNv"
                                "bXB1dGVyIHVudXNhYmxlLiAoTGVzbGllIExhbX"
                                "BvcnQp");
        
        std::string output;
        
        // Act
        encode_base64((const unsigned char *) input, strlen(input), output);

        //Assert
        ASSERT_STREQ(output.c_str(), expected);
}

