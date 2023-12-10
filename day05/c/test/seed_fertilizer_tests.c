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
    printf("scan_ints\n");
    unsigned long my_ints[100];
    int int_max = scan_ints(my_ints, 100, "  42 17 23 4807 ");
    TEST_ASSERT_EQUAL(4, int_max);
    TEST_ASSERT_EQUAL(42, my_ints[0]);
    TEST_ASSERT_EQUAL(17, my_ints[1]);
    TEST_ASSERT_EQUAL(23, my_ints[2]);
    TEST_ASSERT_EQUAL(4807, my_ints[3]);
}

TEST(seed_fertilizer, scan_seeds) {
    printf("scan_seeds\n");
    unsigned long seeds[100];
    int result = scan_seeds(seeds, "seeds: 79 14 55 13");
    TEST_ASSERT_EQUAL(4, result);
    TEST_ASSERT_EQUAL(79, seeds[0]);
    TEST_ASSERT_EQUAL(14, seeds[1]);
    TEST_ASSERT_EQUAL(55, seeds[2]);
    TEST_ASSERT_EQUAL(13, seeds[3]);

}

TEST(seed_fertilizer, scan_converter) {
    printf("scan_converter\n");
    struct Converter converter;
    bool result = scan_converter(&converter, "49 53 8");
    TEST_ASSERT_TRUE(result);
    TEST_ASSERT_EQUAL(49, converter.dest);
    TEST_ASSERT_EQUAL(53, converter.range.start);
    TEST_ASSERT_EQUAL(8, converter.range.len);
    TEST_ASSERT_FALSE(scan_converter(&converter, ""));
}

TEST(seed_fertilizer, read_sample) {
    printf("read_sample\n");
    struct Almanach almanach;
    bool result;
    result = read_almanach(&almanach, "../data/sample.txt");
    TEST_ASSERT_TRUE(result);
    TEST_ASSERT_EQUAL(7, almanach.maxMaps);
    TEST_ASSERT_EQUAL(4, almanach.maps[6].converters[1].range.len);
}
TEST(seed_fertilizer, read_puzzle) {
    printf("read_puzzle\n");
    struct Almanach almanach;
    bool result;
    result = read_almanach(&almanach, "../data/puzzle.txt");
    TEST_ASSERT_TRUE(result);
    TEST_ASSERT_EQUAL(7, almanach.maxMaps);
}

TEST(seed_fertilizer, convert) {
    printf("convert\n");
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
    printf("map\n");
    struct Map map = { 2, .converters = {{ 50, { 98, 2 }}, { 52, { 50, 48 }}}};
    TEST_ASSERT_EQUAL(49, map_convert(49, map));
    TEST_ASSERT_EQUAL(50, map_convert(98, map));
    TEST_ASSERT_EQUAL(52, map_convert(50, map));
    TEST_ASSERT_EQUAL(99, map_convert(97, map));
    TEST_ASSERT_EQUAL(100, map_convert(100,map));
}

TEST(seed_fertilizer, minimum_map_all_seeds) {
    printf("minimum_map_all_seeds\n");
    struct Almanach almanach;
    read_almanach(&almanach, "../data/sample.txt");
    TEST_ASSERT_EQUAL(4, almanach.maxSeeds);
    unsigned long result = minimum_location_map_all_seeds(&almanach);
    TEST_ASSERT_EQUAL(35, result);
}
TEST(seed_fertilizer, solve_puzzle_part_one) {
    printf("solve_puzzle_part_one\n");
    struct Almanach almanach;
    read_almanach(&almanach, "../data/puzzle.txt");
    TEST_ASSERT_EQUAL(20, almanach.maxSeeds);
    unsigned long result = minimum_location_map_all_seeds(&almanach);
    TEST_ASSERT_EQUAL(178159714, result);
}
TEST(seed_fertilizer, seed_ranges) {
    printf("seed_ranges\n");
    struct Almanach almanach;
    read_almanach(&almanach, "../data/sample.txt");
    TEST_ASSERT_EQUAL(2, almanach.maxSeedRanges);

    TEST_ASSERT_EQUAL(79, almanach.seedRanges[0].start);
    TEST_ASSERT_EQUAL(14, almanach.seedRanges[0].len);
    TEST_ASSERT_EQUAL(55, almanach.seedRanges[1].start);
    TEST_ASSERT_EQUAL(13, almanach.seedRanges[1].len);
}

TEST(seed_fertilizer, minimum_map_all_seed_ranges) {
    printf("minimum_map_all_seed_ranges\n");
    struct Almanach almanach;
    read_almanach(&almanach, "../data/sample.txt");
    unsigned long result = minimum_location_map_all_seed_ranges(&almanach);
    TEST_ASSERT_EQUAL(46, result);
}

TEST(seed_fertilizer, solve_puzzle_part_two_brute_force) {
    printf("solve_puzzle_part_two_brute_force\n");
    struct Almanach almanach;
    read_almanach(&almanach, "../data/puzzle.txt");
//    unsigned long result = minimum_location_map_all_seed_ranges(&almanach);
//    TEST_ASSERT_EQUAL(46, result);
}

TEST(seed_fertilizer, id_converter) {
    printf("id_converter\n");
    struct Range range = { 17, 20 };
    struct Converter converter = id_converter(range);
    TEST_ASSERT_EQUAL(17, converter.dest);
    TEST_ASSERT_EQUAL(17, converter.range.start);
    TEST_ASSERT_EQUAL(20, converter.range.len);
}

TEST(seed_fertilizer, split_no_intersect) {
    printf("split_no_intersect\n");
    struct Range range = { 79, 14 };  // extends from 79 to 92, so no intersection
    struct Converter converter = { 50, { 98, 2 }};
    struct Split split = split_converter(id_converter(range), converter);
    printf("initial:");print_converter(id_converter(range));
    printf("converter:");print_converter(converter);
    print_split(split);
    TEST_ASSERT_FALSE(valid_converter(split.before));
    TEST_ASSERT_FALSE(valid_converter(split.intersect));
    TEST_ASSERT_FALSE(valid_converter(split.beyond));

}
TEST(seed_fertilizer, split_full_intersect) {
    printf("split_full_intersect\n");
    struct Range range = { 79, 14 };
    struct Converter converter = { 52, { 79, 14 }};
    printf("initial:");print_converter(id_converter(range));
    printf("converter:");print_converter(converter);
    struct Split split = split_converter(id_converter(range), converter);
    print_split(split);
    TEST_ASSERT_TRUE(valid_converter(split.intersect));
    TEST_ASSERT_EQUAL(52, split.intersect.dest);
    TEST_ASSERT_EQUAL(79, split.intersect.range.start);
    TEST_ASSERT_EQUAL(14, split.intersect.range.len);

    TEST_ASSERT_FALSE(valid_converter(split.before));
    TEST_ASSERT_FALSE(valid_converter(split.beyond));
}
TEST(seed_fertilizer, split_intersect_with_before) {
    printf("split_intersect_with_before\n");
    struct Range range = { 49, 14 };
    struct Converter converter = { 52, { 50, 48 }};
    print_converter(id_converter(range));
    print_converter(converter);
    struct Split split = split_converter(id_converter(range), converter);
    print_split(split);
    TEST_ASSERT_TRUE(valid_converter(split.intersect));
    TEST_ASSERT_EQUAL(81, split.intersect.dest);
    TEST_ASSERT_EQUAL(79, split.intersect.range.start);
    TEST_ASSERT_EQUAL(14, split.intersect.range.len);

    TEST_ASSERT_EQUAL(49, split.before.dest);
    TEST_ASSERT_EQUAL(49, split.before.range.start);
    TEST_ASSERT_EQUAL(30, split.before.range.len);
    TEST_ASSERT_TRUE(valid_converter(split.before));
    TEST_ASSERT_FALSE(valid_converter(split.beyond));
}
TEST(seed_fertilizer, split_intersect_with_beyond) {
    printf("split_intersect_with_beyond\n");
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

    TEST_ASSERT_EQUAL(93, split.beyond.dest);
    TEST_ASSERT_EQUAL(93, split.beyond.range.start);
    TEST_ASSERT_EQUAL(20, split.beyond.range.len);
    TEST_ASSERT_TRUE(valid_converter(split.before));
    TEST_ASSERT_TRUE(valid_converter(split.beyond));
}

TEST(seed_fertilizer, split_map) {
    printf("split_map\n");
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
    printf("map_map\n");
    struct Almanach almanach;
    struct Range range = { 79, 14 };
    read_almanach(&almanach, "../data/sample.txt");
    struct Map source;
    empty_map(&source);
    add_converter(&source, id_converter(range));
    printf("source:\n");
    print_map(&source);
    printf("map[0]:\n");
    print_map(&almanach.maps[0]);
    struct Map result;
    empty_map(&result);
    map_map(&result, &source, &almanach.maps[0]);
    printf("result:\n");
    print_map(&result);
    TEST_ASSERT_EQUAL(1, result.maxConverters);
    TEST_ASSERT_EQUAL(52, result.converters[0].dest);
    TEST_ASSERT_EQUAL(52, result.converters[0].range.start);
    TEST_ASSERT_EQUAL(14, result.converters[0].range.len);
    empty_map(&source);
    append_map(&source, &result);
    empty_map(&result);
    printf("map[1]:\n");
    print_map(&almanach.maps[1]);
    empty_map(&result);
    map_map(&result, &source, &almanach.maps[1]);
    printf("result:\n");
    print_map(&result);
    map_map(&result, &source, &almanach.maps[2]);
}
TEST(seed_fertilizer, map_map_no_split) {
    printf("map_map_no_split\n");
    struct Almanach almanach;
    struct Range range = { 9, 14 };
    read_almanach(&almanach, "../data/sample.txt");
    struct Map source;
    empty_map(&source);
    add_converter(&source, id_converter(range));
    printf("source:\n");
    print_map(&source);
    printf("map[0]:\n");
    print_map(&almanach.maps[0]);
    struct Map result;
    empty_map(&result);
    map_map(&result, &source, &almanach.maps[0]);
    TEST_ASSERT_EQUAL(1, result.maxConverters);
    TEST_ASSERT_EQUAL(9, result.converters[0].dest);
    TEST_ASSERT_EQUAL(9, result.converters[0].range.start);
    TEST_ASSERT_EQUAL(14, result.converters[0].range.len);
    printf("result:\n");
    print_map(&result);
}

TEST(seed_fertilizer, map_all_maps) {
    printf("map_all_maps\n");
    struct Almanach almanach;
    struct Range range;
    read_almanach(&almanach, "../data/sample.txt");
    struct Map result;
    empty_map(&result);
    struct Map source;
    empty_map(&source);
    range = (struct Range){ 79, 14 };
    add_converter(&source, id_converter(range));
    range = (struct Range){ 55, 13 };  
    add_converter(&source, id_converter(range));
    map_all_maps(&result, &source, &almanach);
    printf("result:\n");
    print_map(&result);
}


