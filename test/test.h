#ifndef __TEST_H
#define __TEST_H

#define TEST_SUCCESS 1
#define TEST_FAILURE 0

#define TEST_ASSERT_EQ(a, b) { if (a != b) return TEST_FAILURE; }
#define TEST_ASSERT_NE(a, b) { if (a == b) return TEST_FAILURE; }
#define TEST_ASSERT(a) { if (a) return TEST_FAILURE; }

#include <stdint.h>

typedef uint8_t TestResult;
typedef TestResult (* TestFn)();

typedef struct test_env_t {
    uint16_t count;
    TestFn * tests;
    const char ** descriptions;
} TestEnvironment;

TestEnvironment CreateTestEnvironment();
void AddTest(TestEnvironment *, TestFn, const char *);
TestResult RunTest(TestEnvironment *);

#endif