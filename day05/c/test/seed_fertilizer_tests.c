#include "stdbool.h"
#include "string.h"
#include "unity.h"
#include "unity_fixture.h"
#include "unity_memory.h"
#include "seed_fertilizer.h"

TEST_GROUP(seed_fertilizer);

TEST_SETUP(seed_fertilizer) { }

TEST_TEAR_DOWN(seed_fertilizer) { }


TEST(seed_fertilizer, read_sample) {
    Almanach almanach;
    read_almanach(&almanach, "../data/sample.txt");
    TEST_ASSERT_EQUAL(4, almanach.maps[6].item[1].range.len);
}

TEST(seed_fertilizer, convert_range_no_intersect) {
    RangeSet result;
    result.count = 0;
    Range range = { 79, 14 };
    Converter converter = { 50, { 98, 2 }};
    convert_range(&result, range, converter);
    TEST_ASSERT_EQUAL(0, result.count);
}

TEST(seed_fertilizer, convert_range_intersect) {
    RangeSet result;
    result.count = 0;
    Range range = { 49, 64 };
    Converter converter = { 50, { 79, 14 }};
    convert_range(&result, range, converter);
    TEST_ASSERT_EQUAL(3, result.count);
    TEST_ASSERT_EQUAL(49, result.item[0].start);
    TEST_ASSERT_EQUAL(30, result.item[0].len);
    TEST_ASSERT_EQUAL(50, result.item[1].start);
    TEST_ASSERT_EQUAL(14, result.item[1].len);
    TEST_ASSERT_EQUAL(93, result.item[2].start);
    TEST_ASSERT_EQUAL(20, result.item[2].len);
}

TEST(seed_fertilizer, map_range_no_intersect) {
    RangeSet result;
    result.count = 0;
    Range range = { 1, 2 };
    ConverterSet set;
    add_converter(&set, (Converter){ 50, { 79, 14 }});
    add_converter(&set, (Converter){ 52, { 50, 48 }}); 
    map_convert_range(&result, range, &set);
    TEST_ASSERT_EQUAL(1, result.count);
    TEST_ASSERT_EQUAL(1, result.item[0].start);
    TEST_ASSERT_EQUAL(2, result.item[0].len);
}

TEST(seed_fertilizer, map_range_with_intersect) {
    RangeSet result;
    Range range = { 1, 100 };
    ConverterSet set;
    set.count = 0;
    add_converter(&set, (Converter){ 50, { 98, 2 }}); // 1,97 50,2, 100,1
    add_converter(&set, (Converter){ 52, { 50, 48 }}); 
    map_convert_range(&result, range, &set);
    TEST_ASSERT_EQUAL(6, result.count);
    TEST_ASSERT_EQUAL(1, result.item[0].start); TEST_ASSERT_EQUAL(97, result.item[0].len);
    TEST_ASSERT_EQUAL(50, result.item[1].start); TEST_ASSERT_EQUAL(2, result.item[1].len);
    TEST_ASSERT_EQUAL(100, result.item[2].start); TEST_ASSERT_EQUAL(1, result.item[2].len);
    TEST_ASSERT_EQUAL(1, result.item[3].start); TEST_ASSERT_EQUAL(49, result.item[3].len);
    TEST_ASSERT_EQUAL(52, result.item[4].start); TEST_ASSERT_EQUAL(48, result.item[4].len);
    TEST_ASSERT_EQUAL(98, result.item[5].start); TEST_ASSERT_EQUAL(3, result.item[5].len);
}

TEST(seed_fertilizer, all_maps_range) {
    RangeSet result;
    Range range = { 79, 14 };
    Almanach almanach;
    read_almanach(&almanach, "../data/sample.txt");
    all_maps_range(&result, range, &almanach);
    for(int i = 0; i < result.count; i++) {
        print_range(result.item[i]);
    }
}
