#include "stdbool.h"
#include "string.h"
#include "assert.h"
#include "unity.h"
#include "unity_fixture.h"
#include "unity_memory.h"
#include "seed_fertilizer.h"

Almanach *almanach;
TEST_GROUP(seed_fertilizer);

TEST_SETUP(seed_fertilizer) {
    almanach = new_almanach();
}

TEST_TEAR_DOWN(seed_fertilizer) {
    destroy_almanach(almanach);
}

void check_range(unsigned long, unsigned long, Range);

void check_range(unsigned long start, unsigned long len, Range range) {
    TEST_ASSERT_EQUAL(start, range.start);
    TEST_ASSERT_EQUAL(len, range.len);
}

TEST(seed_fertilizer, read_sample) {
    read_almanach(almanach, "../data/sample.txt");
    TEST_ASSERT_EQUAL(4, almanach->maps[6].converters[1].range.len);
}

TEST(seed_fertilizer, convert_range_no_intersect) {
    RangeSet *result = new_range_set();
    RangeSet *remaining=new_range_set();
    result->count = 0;
    Range range = { 79, 14 };
    Converter converter = { 50, { 98, 2 }};
    convert_range(result, remaining, range, converter);
    check_range(79, 14, remaining->items[0]);
    TEST_ASSERT_EQUAL(0, result->count);
    destroy_range_set(result);
    destroy_range_set(remaining);
}

TEST(seed_fertilizer, convert_range_intersect) {
    RangeSet *result = new_range_set();
    RangeSet *remaining = new_range_set();
    Range range = { 49, 64 };
    Converter converter = { 50, { 79, 14 }};
    convert_range(result, remaining, range, converter);
    TEST_ASSERT_EQUAL(1, result->count);
    check_range(50, 14, result->items[0]);
    check_range(49, 30, remaining->items[0]);
    check_range(93, 20, remaining->items[1]);
    destroy_range_set(result);
    destroy_range_set(remaining);
}

TEST(seed_fertilizer, map_range_no_intersect) {
    read_almanach(almanach, "../data/sample.txt");
    RangeSet *result = new_range_set();
    result->count = 0;
    Range range = { 1, 2 };
    Map map;
    map.count = 0;
    add_converter(&map, (Converter){ 50, { 79, 14 }});
    add_converter(&map, (Converter){ 52, { 50, 48 }}); 
    map_range(result, range, &map);
    TEST_ASSERT_EQUAL(1, result->count);
    check_range(1, 2, result->items[0]);
    destroy_range_set(result);
}
TEST(seed_fertilizer, map_range_with_intersect) {
    read_almanach(almanach, "../data/sample.txt");
    RangeSet *result = new_range_set();
    Range range = { 1, 100 };
    Map map = almanach->maps[0];
    // I1,100 x C50,98,2 → M50,2  (R1,97 R100,1)
    // R1,97  x C52,50,48 → C52,48 (R1,49)
    // R100,1 x C52,50,48 → R100,1
    // R1,49 x ∅ → C1,49
    // R100,1 x ∅ → C100,1
    map_range(result, range, &map);
    TEST_ASSERT_EQUAL(4, result->count);
    TEST_ASSERT_EQUAL(50, result->items[0].start); TEST_ASSERT_EQUAL(2, result->items[0].len);
    TEST_ASSERT_EQUAL(52, result->items[1].start); TEST_ASSERT_EQUAL(48, result->items[1].len);
    TEST_ASSERT_EQUAL(1, result->items[2].start); TEST_ASSERT_EQUAL(49, result->items[2].len);
    TEST_ASSERT_EQUAL(100, result->items[3].start); TEST_ASSERT_EQUAL(1, result->items[3].len);
    destroy_range_set(result);
}
TEST(seed_fertilizer, map_ranges) {
    read_almanach(almanach, "../data/sample.txt");
    RangeSet *result = new_range_set();
    RangeSet *ranges = new_range_set();
    add_range(ranges, (Range) { 79, 14 });
    add_range(ranges, (Range) { 55, 13 });
    Map map;
    for(int m=0; m<MAXMAPS; m++) {
        map = almanach->maps[m];
        map_ranges(result, ranges, &map);
        copy_ranges(ranges, result);
    }
    TEST_ASSERT_EQUAL(6, result->count);
    check_range(46, 11, result->items[0]);
    check_range(82, 3, result->items[1]);
    check_range(86, 4, result->items[2]);
    check_range(94, 3, result->items[3]);
    check_range(56, 4, result->items[4]);
    check_range(97, 2, result->items[5]);
    TEST_ASSERT_EQUAL(46, minimum_start(result));
    destroy_range_set(result);
    destroy_range_set(ranges);
}
TEST(seed_fertilizer, mimimun_range) {
    read_almanach(almanach, "../data/sample.txt");
    unsigned long result = minimum_range_start(almanach);
    TEST_ASSERT_EQUAL(46, result);
}
TEST(seed_fertilizer, solve_puzzle_part_two) {
    read_almanach(almanach, "../data/puzzle.txt");
    unsigned long result = minimum_range_start(almanach);
    TEST_ASSERT_EQUAL(0, result);
}


