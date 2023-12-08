#include "unity_fixture.h"

TEST_GROUP_RUNNER(seed_fertilizer) {
    RUN_TEST_CASE(seed_fertilizer, scan_ints);
    RUN_TEST_CASE(seed_fertilizer, scan_seeds);
    RUN_TEST_CASE(seed_fertilizer, scan_converter);
    RUN_TEST_CASE(seed_fertilizer, scan_map);
    RUN_TEST_CASE(seed_fertilizer, read_almanach);
}
