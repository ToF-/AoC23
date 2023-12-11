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

void TEST_RANGE(unsigned long, unsigned long, Range);

TEST(seed_fertilizer, read_sample) {
    read_almanach(almanach, "../data/sample.txt");
    TEST_ASSERT_EQUAL(4, almanach->maps[6].items[1].range.len);
}

TEST(seed_fertilizer, convert_range_no_intersect) {
    RangeSet *result = new_range_set();
    RangeSet *remaining=new_range_set();
    result->count = 0;
    Range range = { 79, 14 };
    Converter converter = { 50, { 98, 2 }};
    convert_range(result, remaining, range, converter);
    TEST_RANGE(79, 14, remaining->items[0]);
    destroy_range_set(result);
    destroy_range_set(remaining);
}

void TEST_RANGE(unsigned long start, unsigned long len, Range range) {
    TEST_ASSERT_EQUAL(start, range.start);
    TEST_ASSERT_EQUAL(len, range.len);
}

TEST(seed_fertilizer, convert_range_intersect) {
    TEST_IGNORE();
    RangeSet *result = new_range_set();
    RangeSet *remaining = new_range_set();
    Range range = { 49, 64 };
    Converter converter = { 50, { 79, 14 }};
    convert_range(result, remaining, range, converter);
    TEST_ASSERT_EQUAL(1, result->count);
    TEST_RANGE(50, 14, result->items[0]);
    TEST_RANGE(49, 30, remaining->items[0]);
    TEST_RANGE(93, 20, remaining->items[1]);
    destroy_range_set(result);
    destroy_range_set(remaining);
}

TEST(seed_fertilizer, map_range_no_intersect) {
    TEST_IGNORE();
    RangeSet *result = new_range_set();
    result->count = 0;
    Range range = { 1, 2 };
    ConverterSet *set = new_converter_set();
    set->count = 0;
    add_converter(set, (Converter){ 50, { 79, 14 }});
    add_converter(set, (Converter){ 52, { 50, 48 }}); 
    map_convert_range(result, range, set);
    TEST_ASSERT_EQUAL(1, result->count);
    TEST_RANGE(1, 2, result->items[0]);
    destroy_range_set(result);
    destroy_converter_set(set);
}
TEST(seed_fertilizer, map_range_with_intersect) {
    TEST_IGNORE();
    RangeSet *result = new_range_set();
    Range range = { 1, 100 };
    ConverterSet *set = new_converter_set();
    add_converter(set, (Converter){ 50, { 98, 2 }}); // C = 50,2  R = 1,97  100,1
    add_converter(set, (Converter){ 52, { 50, 48 }});  // C 52,48, R = 1,49 100,1
    map_convert_range(result, range, set);
    print_ranges(result);
    TEST_ASSERT_EQUAL(4, result->count);
    TEST_ASSERT_EQUAL(50, result->items[0].start); TEST_ASSERT_EQUAL(2, result->items[0].len);
    TEST_ASSERT_EQUAL(52, result->items[1].start); TEST_ASSERT_EQUAL(48, result->items[1].len);
    TEST_ASSERT_EQUAL(1, result->items[2].start); TEST_ASSERT_EQUAL(49, result->items[2].len);
    TEST_ASSERT_EQUAL(100, result->items[3].start); TEST_ASSERT_EQUAL(1, result->items[3].len);
    destroy_range_set(result);
    destroy_converter_set(set);
}
TEST(seed_fertilizer, map_ranges_all_maps) {
    TEST_IGNORE();
    read_almanach(almanach, "../data/sample.txt");
    Range range = { 79, 14 };
    RangeSet *result = new_range_set();
    map_convert_range_all_maps(result, range, &almanach->maps[0]);
    print_ranges(result);
    range = (Range){ 55, 13 };
    map_convert_range_all_maps(result, range, &almanach->maps[0]);
    print_ranges(result);
    destroy_range_set(result);
}

TEST(seed_fertilizer, convert_range_all_map_level) {
    TEST_IGNORE();
    read_almanach(almanach, "../data/sample.txt");
    map_convert_ranges_all_map_level(almanach->seedRanges, 0, almanach);
    TEST_ASSERT_EQUAL(46, almanach->minimumSeed);
}
TEST(seed_fertilizer, solve_puzzle_part_two) {
    read_almanach(almanach, "../data/puzzle.txt");
    assert(almanach->seedRanges->items[0].start != 79);
    map_convert_ranges_all_map_level(almanach->seedRanges, 0, almanach);
    TEST_ASSERT_EQUAL(46, almanach->minimumSeed);
}
TEST(seed_fertilizer, all_maps_all_ranges) {
    TEST_IGNORE();
    unsigned long result;
    read_almanach(almanach, "../data/sample.txt");
    result = minimum_all_maps_ranges(almanach);
    TEST_ASSERT_EQUAL(46, result);
}


