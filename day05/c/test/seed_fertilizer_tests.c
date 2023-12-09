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
    unsigned long my_ints[100];
    int int_max = scan_ints(my_ints, 100, "  42 17 23 4807 ");
    TEST_ASSERT_EQUAL(4, int_max);
    TEST_ASSERT_EQUAL(42, my_ints[0]);
    TEST_ASSERT_EQUAL(17, my_ints[1]);
    TEST_ASSERT_EQUAL(23, my_ints[2]);
    TEST_ASSERT_EQUAL(4807, my_ints[3]);
}

TEST(seed_fertilizer, scan_seeds) {
    unsigned long seeds[100];
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

TEST(seed_fertilizer, read_sample) {
    struct Almanach almanach;
    bool result;
    result = read_almanach(&almanach, "../data/sample.txt");
    TEST_ASSERT_TRUE(result);
    TEST_ASSERT_EQUAL(7, almanach.maxMaps);
    TEST_ASSERT_EQUAL(4, almanach.maps[6].converters[1].range.len);
}
TEST(seed_fertilizer, read_puzzle) {
    struct Almanach almanach;
    bool result;
    result = read_almanach(&almanach, "../data/puzzle.txt");
    TEST_ASSERT_TRUE(result);
    TEST_ASSERT_EQUAL(7, almanach.maxMaps);
}

TEST(seed_fertilizer, convert) {
    struct Converter converter = { 50, { 98, 2 }};
    TEST_ASSERT_EQUAL(50, convert(98, converter));
    TEST_ASSERT_EQUAL(51, convert(99, converter));
    TEST_ASSERT_EQUAL(INVALID, convert(100, converter));
    TEST_ASSERT_EQUAL(INVALID, convert(97, converter));
}

         // ,"50 98 2"
         // ,"52 50 48"
         //    let m = (maps sf)!!0
         //    mapping 49 m `shouldBe` 49
         //    mapping 98 m `shouldBe` 50
         //    mapping 50 m `shouldBe` 52
         //    mapping 97 m `shouldBe` 99
         //    mapping 100 m `shouldBe` 100

TEST(seed_fertilizer, map) {
    struct Map map = { 2, .converters = {{ 50, { 98, 2 }}, { 52, { 50, 48 }}}};
    TEST_ASSERT_EQUAL(49, map_convert(49, map));
    TEST_ASSERT_EQUAL(50, map_convert(98, map));
    TEST_ASSERT_EQUAL(52, map_convert(50, map));
    TEST_ASSERT_EQUAL(99, map_convert(97, map));
    TEST_ASSERT_EQUAL(100, map_convert(100,map));
}

TEST(seed_fertilizer, minimum_map_all_seeds) {
    struct Almanach almanach;
    read_almanach(&almanach, "../data/sample.txt");
    TEST_ASSERT_EQUAL(4, almanach.maxSeeds);
    unsigned long result = minimum_location_map_all_seeds(&almanach);
    TEST_ASSERT_EQUAL(35, result);
}
TEST(seed_fertilizer, solve_puzzle_part_one) {
    struct Almanach almanach;
    read_almanach(&almanach, "../data/puzzle.txt");
    TEST_ASSERT_EQUAL(20, almanach.maxSeeds);
    unsigned long result = minimum_location_map_all_seeds(&almanach);
    TEST_ASSERT_EQUAL(178159714, result);
}
TEST(seed_fertilizer, seed_ranges) {
    struct Almanach almanach;
    read_almanach(&almanach, "../data/sample.txt");
    TEST_ASSERT_EQUAL(2, almanach.maxSeedRanges);

    TEST_ASSERT_EQUAL(79, almanach.seedRanges[0].start);
    TEST_ASSERT_EQUAL(14, almanach.seedRanges[0].len);
    TEST_ASSERT_EQUAL(55, almanach.seedRanges[1].start);
    TEST_ASSERT_EQUAL(13, almanach.seedRanges[1].len);
}

TEST(seed_fertilizer, minimum_map_all_seed_ranges) {
    struct Almanach almanach;
    read_almanach(&almanach, "../data/sample.txt");
    unsigned long result = minimum_location_map_all_seed_ranges(&almanach);
    TEST_ASSERT_EQUAL(46, result);
}

TEST(seed_fertilizer, solve_puzzle_part_two_brute_force) {
    struct Almanach almanach;
    read_almanach(&almanach, "../data/puzzle.txt");
//    unsigned long result = minimum_location_map_all_seed_ranges(&almanach);
//    TEST_ASSERT_EQUAL(46, result);
}

TEST(seed_fertilizer, id_converter) {
    struct Range range = { 17, 20 };
    struct Converter converter = id_converter(range);
    TEST_ASSERT_EQUAL(17, converter.dest);
    TEST_ASSERT_EQUAL(17, converter.range.start);
    TEST_ASSERT_EQUAL(20, converter.range.len);
}

TEST(seed_fertilizer, split_no_intersect) {
    struct Range range = { 79, 14 };  // extends from 79 to 92, so no intersection
    struct Converter converter = { 50, { 98, 2 }};
    struct Split split = split_converter(id_converter(range), converter);
    print_converter(id_converter(range));
    print_converter(converter);
    print_split(split);
    TEST_ASSERT_EQUAL(79, split.original.dest);
    TEST_ASSERT_EQUAL(79, split.original.range.start);
    TEST_ASSERT_EQUAL(14, split.original.range.len);
    TEST_ASSERT_TRUE(valid_converter(split.original));
    TEST_ASSERT_FALSE(valid_converter(split.before));
    TEST_ASSERT_FALSE(valid_converter(split.intersect));
    TEST_ASSERT_FALSE(valid_converter(split.beyond));
}
TEST(seed_fertilizer, split_full_intersect) {
    struct Range range = { 79, 14 };
    struct Converter converter = { 50, { 79, 14 }};
    print_converter(id_converter(range));
    print_converter(converter);
    struct Split split = split_converter(id_converter(range), converter);
    print_split(split);
    TEST_ASSERT_TRUE(valid_converter(split.intersect));
    TEST_ASSERT_EQUAL(50, split.intersect.dest);
    TEST_ASSERT_EQUAL(79, split.intersect.range.start);
    TEST_ASSERT_EQUAL(14, split.intersect.range.len);

    TEST_ASSERT_FALSE(valid_converter(split.original));
    TEST_ASSERT_FALSE(valid_converter(split.before));
    TEST_ASSERT_FALSE(valid_converter(split.beyond));
}
TEST(seed_fertilizer, split_intersect_with_before) {
    struct Range range = { 49, 44 };
    struct Converter converter = { 50, { 79, 14 }};
    print_converter(id_converter(range));
    print_converter(converter);
    struct Split split = split_converter(id_converter(range), converter);
    print_split(split);
    TEST_ASSERT_TRUE(valid_converter(split.intersect));
    TEST_ASSERT_EQUAL(50, split.intersect.dest);
    TEST_ASSERT_EQUAL(79, split.intersect.range.start);
    TEST_ASSERT_EQUAL(14, split.intersect.range.len);

    TEST_ASSERT_FALSE(valid_converter(split.original));
    TEST_ASSERT_EQUAL(49, split.before.dest);
    TEST_ASSERT_EQUAL(49, split.before.range.start);
    TEST_ASSERT_EQUAL(30, split.before.range.len);
    TEST_ASSERT_TRUE(valid_converter(split.before));
    TEST_ASSERT_FALSE(valid_converter(split.beyond));
}
TEST(seed_fertilizer, split_intersect_with_beyond) {
    struct Range range = { 49, 64 };
    struct Converter converter = { 50, { 79, 14 }};
    print_converter(id_converter(range));
    print_converter(converter);
    struct Split split = split_converter(id_converter(range), converter);
    print_split(split);
    TEST_ASSERT_TRUE(valid_converter(split.intersect));
    TEST_ASSERT_EQUAL(50, split.intersect.dest);
    TEST_ASSERT_EQUAL(79, split.intersect.range.start);
    TEST_ASSERT_EQUAL(14, split.intersect.range.len);

    TEST_ASSERT_FALSE(valid_converter(split.original));
    TEST_ASSERT_EQUAL(93, split.beyond.dest);
    TEST_ASSERT_EQUAL(93, split.beyond.range.start);
    TEST_ASSERT_EQUAL(20, split.beyond.range.len);
    TEST_ASSERT_TRUE(valid_converter(split.before));
    TEST_ASSERT_TRUE(valid_converter(split.beyond));
}

TEST(seed_fertilizer, split_map) {
    struct Range range = { 49, 64 };
    struct Converter converter = { 50, { 79, 14 }};
    struct Split split = split_converter(id_converter(range), converter);
    print_split(split);
    struct Map result;
    split_map(&result, split);
    TEST_ASSERT_EQUAL(3, result.maxConverters);
    TEST_ASSERT_EQUAL(49, result.converters[0].dest);
    TEST_ASSERT_EQUAL(49, result.converters[0].range.start);
    TEST_ASSERT_EQUAL(30, result.converters[0].range.len);
    TEST_ASSERT_EQUAL(50, result.converters[1].dest);
    TEST_ASSERT_EQUAL(50, result.converters[1].range.start);
    TEST_ASSERT_EQUAL(14, result.converters[1].range.len);
    TEST_ASSERT_EQUAL(93, result.converters[2].dest);
    TEST_ASSERT_EQUAL(93, result.converters[2].range.start);
    TEST_ASSERT_EQUAL(20, result.converters[2].range.len);
    print_map(&result);
}

TEST(seed_fertilizer, map_map) {
    struct Almanach almanach;
    struct Range range = { 79, 14 };
    read_almanach(&almanach, "../data/sample.txt");
    struct Map source;
    source.converters[0] = id_converter(range);
    source.maxConverters = 1;
    struct Map result;
    struct Map first = almanach.maps[0];
    map_map(&result, &source, &first);
}

