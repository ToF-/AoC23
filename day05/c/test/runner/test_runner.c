#include "unity_fixture.h"

TEST_GROUP_RUNNER(seed_fertilizer) {
    RUN_TEST_CASE(seed_fertilizer, scan_ints);
    RUN_TEST_CASE(seed_fertilizer, scan_seeds);
    RUN_TEST_CASE(seed_fertilizer, scan_converter);
    RUN_TEST_CASE(seed_fertilizer, read_sample);
    RUN_TEST_CASE(seed_fertilizer, read_puzzle);
    RUN_TEST_CASE(seed_fertilizer, convert);
    RUN_TEST_CASE(seed_fertilizer, map);
    RUN_TEST_CASE(seed_fertilizer, minimum_map_all_seeds);
    RUN_TEST_CASE(seed_fertilizer, solve_puzzle_part_one);
    RUN_TEST_CASE(seed_fertilizer, seed_ranges);
    RUN_TEST_CASE(seed_fertilizer, minimum_map_all_seed_ranges);
    RUN_TEST_CASE(seed_fertilizer, solve_puzzle_part_two_brute_force);
    RUN_TEST_CASE(seed_fertilizer, id_converter);
    RUN_TEST_CASE(seed_fertilizer, split_no_intersect);
    RUN_TEST_CASE(seed_fertilizer, split_full_intersect);
    RUN_TEST_CASE(seed_fertilizer, split_intersect_with_before);
    RUN_TEST_CASE(seed_fertilizer, split_intersect_with_beyond);
    RUN_TEST_CASE(seed_fertilizer, split_map);
    RUN_TEST_CASE(seed_fertilizer, map_map);
    RUN_TEST_CASE(seed_fertilizer, map_map_no_split);
    RUN_TEST_CASE(seed_fertilizer, map_all_maps);
}
