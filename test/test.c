#include "test.h"

#include <stdio.h>
#include <stdlib.h>

TestEnvironment CreateTestEnvironment() {
    TestEnvironment environment = {
            .count = 0,
            .tests = malloc(0),
            .descriptions = malloc(0)
    };
    return environment;
}

void AddTest(TestEnvironment * environment, TestFn fn, const char * description) {
    TestFn * tests = malloc((environment->count + 1) * sizeof(TestFn));
    const char ** descriptions = malloc((environment->count + 1) * sizeof(char *));
    for (uint16_t i = 0; i < environment->count; i++) {
        tests[i] = environment->tests[i];
        descriptions[i] = environment->descriptions[i];
    }
    tests[environment->count] = fn;
    descriptions[environment->count] = description;
    environment->count++;
    free(environment->tests);
    free(environment->descriptions);
    environment->tests = tests;
    environment->descriptions = descriptions;
}

TestResult RunTest(TestEnvironment * environment) {
    TestResult result = TEST_SUCCESS;
    for (uint16_t i = 0; i < environment->count; i++) {
        printf("Running test #%d '%s'... ", i, environment->descriptions[i]);
        if (environment->tests[i]() == TEST_FAILURE) {
            result = TEST_FAILURE;
            printf("\033[31mFAILED\033[0m\n");
        } else {
            printf("\033[32mSUCCESS\033[0m\n");
        }
    }
    return result;
}