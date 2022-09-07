#include <string>

#include "gtest/gtest.h"
#include "rcom/MemBuffer.h"

class MemBuffer_tests : public ::testing::Test
{
protected:
        MemBuffer_tests() = default;

        ~MemBuffer_tests() override = default;

        void SetUp() override
                {
                }

        void TearDown() override
                {

                }

};

TEST_F(MemBuffer_tests, can_construct)
{
        // Arrange
        rcom::MemBuffer MemBuffer;

        // Act
        auto buffer = MemBuffer.data();

        //Assert
        ASSERT_EQ(buffer.size(), 0);
}

TEST_F(MemBuffer_tests, can_copy_construct)
{
    // Arrange
    rcom::MemBuffer MemBufferOrig;
    std::string expected("teststring");
    MemBufferOrig.append(expected);

    rcom::MemBuffer MemBufferCopy(MemBufferOrig);

    // Act
    auto actual = MemBufferCopy.tostring();

    //Assert
    ASSERT_EQ(actual, expected);
}

TEST_F(MemBuffer_tests, equals_operator_correct_when_equal)
{
    // Arrange
    bool expected = true;
    std::string data("teststring");
    rcom::MemBuffer MemBufferL;
    rcom::MemBuffer MemBufferR;
    MemBufferL.append(data);
    MemBufferR.append(data);

    // Act
    auto actual = (MemBufferL == MemBufferR);

    //Assert
    ASSERT_EQ(actual, expected);
}

TEST_F(MemBuffer_tests, equals_operator_correct_when_not_equal)
{
    // Arrange
    bool expected = false;
    std::string data("teststring");
    rcom::MemBuffer MemBufferL;
    rcom::MemBuffer MemBufferR;
    MemBufferL.append(data);

    // Act

    auto actual = (MemBufferL == MemBufferR);

    //Assert
    ASSERT_EQ(actual, expected);
}

TEST_F(MemBuffer_tests, put_appends_char)
{
        // Arrange
        rcom::MemBuffer MemBuffer;
        std::string expected("test");

        // Act
        MemBuffer.put('t');
        MemBuffer.put('e');
        MemBuffer.put('s');
        MemBuffer.put('t');


        auto buffer = MemBuffer.data();
        std::string actual(buffer.begin(), buffer.end());

        //Assert
        ASSERT_EQ(actual, expected);
}

 TEST_F(MemBuffer_tests, append_appends_chars)
 {
         // Arrange
         rcom::MemBuffer MemBuffer;
         const int bufflen = 4;
         uint8_t buff[bufflen] = { 't', 'e', 's', 't'};

         // Act
         MemBuffer.append(buff, bufflen);

         auto actual = MemBuffer.data();

         //Assert
         ASSERT_EQ(actual[0], 't');
         ASSERT_EQ(actual[1], 'e');
         ASSERT_EQ(actual[2], 's');
         ASSERT_EQ(actual[3], 't');
 }

 TEST_F(MemBuffer_tests, append_appends_len_chars)
 {
         // Arrange
         rcom::MemBuffer MemBuffer;
         const int bufflen = 4;
         uint8_t buff[bufflen+1] = { 't', 'e', 's', 't', 's'};

         // Act
         MemBuffer.append(buff, bufflen);

         auto actual = MemBuffer.data();

         //Assert
         ASSERT_EQ(actual.size(), bufflen);
         ASSERT_EQ(actual[0], 't');
         ASSERT_EQ(actual[1], 'e');
         ASSERT_EQ(actual[2], 's');
         ASSERT_EQ(actual[3], 't');
 }

TEST_F(MemBuffer_tests, append_string_appends_string)
{
        // Arrange
        std::string expected("teststring");
        rcom::MemBuffer MemBuffer;

        // Act
        MemBuffer.append_string_truncates_to_32k(expected.c_str());
        auto actual = MemBuffer.tostring();

        //Assert
        ASSERT_EQ(actual, expected);
}

TEST_F(MemBuffer_tests, append_string_appends_string_2)
{
        // Arrange
        std::string expected("teststring");
        rcom::MemBuffer MemBuffer;

        // Act
        MemBuffer.append(expected);
        auto actual = MemBuffer.tostring();

        //Assert
        ASSERT_EQ(actual, expected);
}

TEST_F(MemBuffer_tests, append_string_truncates_to_32k)
{
        // Arrange
        const int KB32 = 1024 * 32;
        const int large_string_size = (KB32 + 2);
        char large_string[large_string_size];
        memset(large_string, 'a', large_string_size);
        large_string[large_string_size -1] = 0;

        rcom::MemBuffer MemBuffer;

        // Act
        MemBuffer.append_string_32k_max(large_string);
        auto actual = MemBuffer.data();

        //Assert
        ASSERT_EQ(actual.size(), KB32);
}

TEST_F(MemBuffer_tests, print_f_prints_to_membuffer)
{
        // Arrange
        std::string expected("teststring");
        rcom::MemBuffer MemBuffer;

        // Act
        MemBuffer.printf("%s", expected.c_str());
        auto actual = MemBuffer.tostring();

        //Assert
        ASSERT_EQ(actual, expected);
}

TEST_F(MemBuffer_tests, data_returns_data)
{
        // Arrange
        std::string expected("test");
        rcom::MemBuffer MemBuffer;

        // Act
        MemBuffer.printf("%s", expected.c_str());
        auto actual = MemBuffer.data();

        //Assert
        ASSERT_EQ(actual.size(), expected.size());
        ASSERT_EQ(actual[0], 't');
        ASSERT_EQ(actual[1], 'e');
        ASSERT_EQ(actual[2], 's');
        ASSERT_EQ(actual[3], 't');
}

TEST_F(MemBuffer_tests, string_returns_string)
{
        // Arrange
        std::string expected("teststring");
        rcom::MemBuffer MemBuffer;

        // Act
        MemBuffer.printf("%s", expected.c_str());
        auto actual = MemBuffer.tostring();

        //Assert
        ASSERT_EQ(actual, expected);
}

TEST_F(MemBuffer_tests, clear_clears_data_size_correct)
{
        // Arrange
        std::string expected("teststring");
        rcom::MemBuffer MemBuffer;

        // Act
        MemBuffer.printf("%s", expected.c_str());
        auto expected_string = MemBuffer.tostring();
        MemBuffer.clear();
        auto actual = MemBuffer.data();
        auto actual_size = MemBuffer.size();

        //Assert
        ASSERT_EQ(expected_string, expected);
        ASSERT_EQ(actual_size, 0);
}
