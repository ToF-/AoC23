C_COMPILER=gcc

U=unity

TEST_TARGET=bin/run_test

TEST_FILES=test/*.c test/runner/*.c

SRC_FILES=src/trebuchet.c $(U)/src/unity.c $(U)/extras/memory/src/unity_memory.c $(U)/extras/fixture/src/unity_fixture.c $(TEST_FILES)

INC_DIRS=-Isrc -I$(U)/src -I$(U)/extras/fixture/src -I$(U)/extras/memory/src

tests:
	mkdir -p bin
	mkdir -p test/runner
	./gen_test_runner.awk test/tests.c >test/runner/test_runner.c
	./gen_all_tests.awk   test/tests.c >test/runner/all_tests.c
	$(C_COMPILER) $(CFLAGS) $(INC_DIRS) $(SRC_FILES) -o bin/run_test
	- bin/run_test
