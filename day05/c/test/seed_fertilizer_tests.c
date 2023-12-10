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
    printf("read_sample\n");
    Almanach almanach;
    read_almanach(&almanach, "../data/sample.txt");
    TEST_ASSERT_EQUAL(4, almanach.maps[6].item[1].range.len);
}
