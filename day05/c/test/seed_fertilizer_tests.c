#include "stdbool.h"
#include "string.h"
#include "unity.h"
#include "unity_fixture.h"
#include "unity_memory.h"
#include "seed_fertilizer.h"

TEST_GROUP(seed_fertilizer);

TEST_SETUP(seed_fertilizer) { }

TEST_TEAR_DOWN(seed_fertilizer) { }

TEST(seed_fertilizer, scan_ints) {
    long my_ints[100];
    int int_max = scan_ints(my_ints, 100, "  42 17 23 4807 ");
    TEST_ASSERT_EQUAL(4, int_max);
    TEST_ASSERT_EQUAL(42, my_ints[0]);
    TEST_ASSERT_EQUAL(17, my_ints[1]);
    TEST_ASSERT_EQUAL(23, my_ints[2]);
    TEST_ASSERT_EQUAL(4807, my_ints[3]);
}

TEST(seed_fertilizer, scan_seeds) {
    long seeds[100];
    int result = scan_seeds(seeds, "seeds: 79 14 55 13");
    TEST_ASSERT_EQUAL(4, result);
    TEST_ASSERT_EQUAL(79, seeds[0]);
    TEST_ASSERT_EQUAL(14, seeds[1]);
    TEST_ASSERT_EQUAL(55, seeds[2]);
    TEST_ASSERT_EQUAL(13, seeds[3]);

}

TEST(seed_fertilizer, scan_converter) {
    struct Converter converter;
    bool result = scan_converter(&converter, "49 53 8");
    TEST_ASSERT_TRUE(result);
    TEST_ASSERT_EQUAL(49, converter.dest);
    TEST_ASSERT_EQUAL(53, converter.range.start);
    TEST_ASSERT_EQUAL(8, converter.range.len);
    TEST_ASSERT_FALSE(scan_converter(&converter, ""));
}

TEST(seed_fertilizer, read_almanach) {
    struct Almanach almanach;
    bool result;
    result = read_almanach(&almanach, "../data/sample.txt");
    result = read_almanach(&almanach, "../data/sample.txt");
    TEST_ASSERT_TRUE(result);
    TEST_ASSERT_EQUAL(7, almanach.maxMaps);
    TEST_ASSERT_EQUAL(4, almanach.maps[6].converters[1].range.len);
}
