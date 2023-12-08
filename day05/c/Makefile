CLEANUP = rm -f

C_COMPILER=gcc
ifeq ($(shell uname -s), Darwin)
C_COMPILER=clang
endif

UNITY_ROOT=unity

TEST_TARGET=bin/run_test

TEST_FILES=test/*.c test/runner/*.c

SRC_FILES=src/seed_fertilizer.c $(UNITY_ROOT)/src/unity.c $(UNITY_ROOT)/extras/memory/src/unity_memory.c $(UNITY_ROOT)/extras/fixture/src/unity_fixture.c  $(TEST_FILES)

INC_DIRS=-Isrc -I$(UNITY_ROOT)/src -I$(UNITY_ROOT)/extras/fixture/src -I$(UNITY_ROOT)/extras/memory/src

tests:
	mkdir -p test/runner
	./gen_test_runner.awk test/seed_fertilizer_tests.c >test/runner/test_runner.c
	./gen_all_tests.awk test/seed_fertilizer_tests.c >test/runner/all_tests.c
	$(C_COMPILER) $(CFLAGS) $(INC_DIRS) $(SYMBOLS) $(SRC_FILES) -o $(TEST_TARGET)
	- ./$(TEST_TARGET)

app: src/seed_fertilizer.c
	mkdir -p bin
	cc src/main.c src/seed_fertilizer.c -o bin/seed_fertilizer

clean:
	rm bin/*

