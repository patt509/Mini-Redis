#ifndef TEST_FRAMEWORK_H
#define TEST_FRAMWORK_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// ANSI Color codes for terminal output
#define GREEN "\033[0;32m"
#define RED   "\033[0;31m"
#define RESET "\033[0m"

// Global counters for test statistics
extern int tests_run;
extern int tests_passed;
extern int tests_failed;

/*
 * ASSERT Macro
 * Checks if a condition is true. If not, prints an error message and increments fail count.
 */
#define ASSERT(condition, message) do { \
   tests_run++; \
   if (!(condition)) { \
      printf(RED "[FAIL] %s\n" RESET, message); \
      printf("       File: %s, Line: %d\n", __FILE__, __LINE__); \
      tests_failed++; \
      return; \
   } else { \
      tests_passed++; \
   } \
} while(0)

/*
 * ASSERT_STR_EQ Macro
 * Specialized assertion for string equality (wrapper around strcmp).
 */
#define ASSERT_STR_EQ(actual, expected, message) do { \
   tests_run++; \
   if (actual == NULL || expected == NULL || strcmp(actual, expected) != 0) { \
      printf(RED "[FAIL] %s\n" RESET, message); \
      printf("       Expected: '%s', Got: '%s'\n", expected, actual ? actual : "NULL"); \
      tests_failed++; \
      return; \
   } else { \
      tests_passed++; \
   } \
} while(0)

#endif