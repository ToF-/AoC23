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
    int my_ints[100];
    int int_max = scan_ints(my_ints, 100, "  42 17 23 4807 ");
    TEST_ASSERT_EQUAL(4, int_max);
    TEST_ASSERT_EQUAL(42, my_ints[0]);
    TEST_ASSERT_EQUAL(17, my_ints[1]);
    TEST_ASSERT_EQUAL(23, my_ints[2]);
    TEST_ASSERT_EQUAL(4807, my_ints[3]);
}

TEST(seed_fertilizer, scan_seeds) {
    int seeds[100];
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

TEST(seed_fertilizer, scan_map) {
    struct Map map;
    char *lines[5];
    lines[0] = strdup("soil-to-fertilizer map:");
    lines[1] = strdup("0 15 37");
    lines[2] = strdup("37 52 2");
    lines[3] = strdup("39 0 15");
    lines[4] = strdup("");

    int result = scan_map(&map, lines, 0);
    TEST_ASSERT_EQUAL(3, result);
    TEST_ASSERT_EQUAL(3, map.maxConverters);
    TEST_ASSERT_EQUAL(0, map.converters[0].dest);
    TEST_ASSERT_EQUAL(52, map.converters[1].range.start);
    TEST_ASSERT_EQUAL(15, map.converters[2].range.len);
}
TEST(seed_fertilizer, read_almanach) {
    struct Almanach almanach;
    bool result = read_almanach(&almanach, "../data/sample.txt");
    TEST_ASSERT_TRUE(result);
    TEST_ASSERT_EQUAL(7, almanach.maxMaps);
    TEST_ASSERT_EQUAL(4, almanach.maps[6].converters[1].range.len);



}

