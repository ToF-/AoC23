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
}
