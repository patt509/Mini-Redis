#include <stdio.h>
#include "test_ht.h"
#include "test_framework.h"

int main() {
    printf("=================================\n");
    printf("    MINI-REDIS TEST SUITE        \n");
    printf("=================================\n\n");

    // Run individual test suites
    run_hashtable_tests();

    // Final Report
    printf("---------------------------------\n");
    printf("Tests Run:    %d\n", tests_run);
    if (tests_failed == 0) {
        printf(GREEN "PASSED:       %d\n" RESET, tests_passed);
        printf(GREEN "\nALL TESTS PASSED SUCCESSFULLY! 🚀\n" RESET);
    } else {
        printf(GREEN "PASSED:       %d\n" RESET, tests_passed);
        printf(RED "FAILED:       %d\n" RESET, tests_failed);
        printf(RED "\nSOME TESTS FAILED. CHECK OUTPUT ABOVE. ❌\n" RESET);
    }
    printf("---------------------------------\n");

    return tests_failed > 0 ? 1 : 0;
}