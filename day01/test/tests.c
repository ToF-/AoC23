#include "unity.h"
#include "unity_fixture.h"
#include "unity_memory.h"
#include "trebuchet.h"

TEST_GROUP(trebuchet);

TEST_SETUP(trebuchet) { }

TEST_TEAR_DOWN(trebuchet) { }

TEST(trebuchet, dummy) {
  int result;
  result = doit();
  TEST_ASSERT_EQUAL(42, result);
}
