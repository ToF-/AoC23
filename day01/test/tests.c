#include "unity.h"
#include "unity_fixture.h"
#include "unity_memory.h"
#include "trebuchet.h"

TEST_GROUP(trebuchet);

TEST_SETUP(trebuchet) {
    read_puzzle("data/puzzle.txt");
}

TEST_TEAR_DOWN(trebuchet) {
    free_puzzle();
}

TEST(trebuchet, read_puzzle) {
    TEST_ASSERT_EQUAL_INT(1000, Max_Lines);
    TEST_ASSERT_EQUAL_STRING("cmpptgjc3qhcjxcbcqgqkxhrms", Lines[0]);
    TEST_ASSERT_EQUAL_STRING("9kxgfxntgdzs1ns5", Lines[10]);
}

TEST(trebuchet, calibration_value_for_digits) {
    TEST_ASSERT_EQUAL_INT(12, digit_calibration_value("1abc2", 0));
    TEST_ASSERT_EQUAL_INT(15, digit_calibration_value("a1b2c3d4e5f", 0));
    TEST_ASSERT_EQUAL_INT(77, digit_calibration_value("treb7uchet", 0));
}

TEST(trebuchet, spelled_and_digit_calibration_value) {
    TEST_ASSERT_EQUAL_INT(12, digit_calibration_value("1abc2", 1));
    TEST_ASSERT_EQUAL_INT(13, digit_calibration_value("abcone2threexyz", 1));
}

TEST(trebuchet, sum_calibration_values) {
    TEST_ASSERT_EQUAL_INT(54916, sum_calibration_values(0));
    TEST_ASSERT_EQUAL_INT(54728, sum_calibration_values(1));
}

