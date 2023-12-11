#include "stdbool.h"
#include "string.h"
#include "assert.h"
#include "unity.h"
#include "unity_fixture.h"
#include "unity_memory.h"
#include "seed_fertilizer.h"

TEST_GROUP(seed_fertilizer);

TEST_SETUP(seed_fertilizer) { }

TEST_TEAR_DOWN(seed_fertilizer) { }

void TEST_RANGE(unsigned long, unsigned long, Range);

TEST(seed_fertilizer, read_sample) {
    Almanach almanach;
    read_almanach(&almanach, "../data/sample.txt");
    TEST_ASSERT_EQUAL(4, almanach.maps[6].item[1].range.len);
}

TEST(seed_fertilizer, convert_range_no_intersect) {
    RangeSet result;
    RangeSet remaining;
    result.count = 0;
    Range range = { 79, 14 };
    Converter converter = { 50, { 98, 2 }};
    convert_range(&result, &remaining, range, converter);
    TEST_RANGE(79, 14, remaining.item[0]);
}

void TEST_RANGE(unsigned long start, unsigned long len, Range range) {
    TEST_ASSERT_EQUAL(start, range.start);
    TEST_ASSERT_EQUAL(len, range.len);
}

TEST(seed_fertilizer, convert_range_intersect) {
    RangeSet result;
    RangeSet remaining;
    result.count = 0;
    remaining.count = 0;
    Range range = { 49, 64 };
    Converter converter = { 50, { 79, 14 }};
    convert_range(&result, &remaining, range, converter);
    TEST_ASSERT_EQUAL(1, result.count);
    TEST_RANGE(50, 14, result.item[0]);
    TEST_RANGE(49, 30, remaining.item[0]);
    TEST_RANGE(93, 20, remaining.item[1]);
}

TEST(seed_fertilizer, map_range_no_intersect) {
    RangeSet result;
    result.count = 0;
    Range range = { 1, 2 };
    ConverterSet set = new_ConverterSet();
    set.count = 0;
    add_converter(&set, (Converter){ 50, { 79, 14 }});
    add_converter(&set, (Converter){ 52, { 50, 48 }}); 
    map_convert_range(&result, range, &set);
    TEST_ASSERT_EQUAL(1, result.count);
    TEST_RANGE(1, 2, result.item[0]);
}
TEST(seed_fertilizer, map_range_with_intersect) {
    RangeSet result;
    result.count = 0;
    Range range = { 1, 100 };
    ConverterSet set = new_ConverterSet();
    add_converter(&set, (Converter){ 50, { 98, 2 }}); // C = 50,2  R = 1,97  100,1
    add_converter(&set, (Converter){ 52, { 50, 48 }});  // C 52,48, R = 1,49 100,1
    map_convert_range(&result, range, &set);
    print_ranges(&result);
    TEST_ASSERT_EQUAL(4, result.count);
    TEST_ASSERT_EQUAL(50, result.item[0].start); TEST_ASSERT_EQUAL(2, result.item[0].len);
    TEST_ASSERT_EQUAL(52, result.item[1].start); TEST_ASSERT_EQUAL(48, result.item[1].len);
    TEST_ASSERT_EQUAL(1, result.item[2].start); TEST_ASSERT_EQUAL(49, result.item[2].len);
    TEST_ASSERT_EQUAL(100, result.item[3].start); TEST_ASSERT_EQUAL(1, result.item[3].len);
}
TEST(seed_fertilizer, map_ranges_all_maps) {
    TEST_IGNORE();
    Almanach almanach;
    read_almanach(&almanach, "../data/sample.txt");
    Range range = { 79, 14 };
    RangeSet result;
    map_convert_range_all_maps(&result, range, &almanach.maps[0]);
    print_ranges(&result);
    getchar();
    range = (Range){ 55, 13 };
    map_convert_range_all_maps(&result, range, &almanach.maps[0]);
    print_ranges(&result);
}

TEST(seed_fertilizer, convert_range_all_map_level) {
    Almanach almanach;
    read_almanach(&almanach, "../data/sample.txt");
    RangeSet result;
    empty_ranges(&result);
    map_convert_ranges_all_map_level(&result, &almanach.seedRanges, 0, &almanach);
    TEST_ASSERT_EQUAL(46, minimum(&result));
}
TEST(seed_fertilizer, solve_puzzle_part_two) {
    Almanach *al = (Almanach *)malloc(sizeof(Almanach));
    read_almanach(al, "../data/puzzle.txt");
    assert(al->seedRanges.item[0].start != 79);
    RangeSet result;
    empty_ranges(&result);
    map_convert_ranges_all_map_level(&result, &al->seedRanges, 0, al);
    TEST_ASSERT_EQUAL(46, minimum(&result));
    free(al);
}
TEST(seed_fertilizer, all_maps_all_ranges) {
    TEST_IGNORE();
    unsigned long result;
    Almanach almanach;
    read_almanach(&almanach, "../data/sample.txt");
    result = minimum_all_maps_ranges(&almanach);
    TEST_ASSERT_EQUAL(46, result);
}


