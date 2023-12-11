#include "unity_fixture.h"

TEST_GROUP_RUNNER(seed_fertilizer) {
    RUN_TEST_CASE(seed_fertilizer, read_sample);
    RUN_TEST_CASE(seed_fertilizer, convert_range_no_intersect);
    RUN_TEST_CASE(seed_fertilizer, convert_range_intersect);
    RUN_TEST_CASE(seed_fertilizer, map_range_no_intersect);
    RUN_TEST_CASE(seed_fertilizer, map_range_with_intersect);
    RUN_TEST_CASE(seed_fertilizer, all_maps_all_ranges);
}
