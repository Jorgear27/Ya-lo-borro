#include "commands.h"
#include "unity.h"

/**
 * @brief Buffer size for the tests
 *
 */
#define CWDBUF 1024

/**
 * @brief Test the change_directory function
 *
 */
void test_change_directory(void);

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_change_directory);
    return UNITY_END();
}

void test_change_directory(void) {
    // Change to /tmp directory
    change_directory("/tmp");

    // Check if the current directory is /tmp
    char cwd[CWDBUF];
    getcwd(cwd, sizeof(cwd));
    TEST_ASSERT_EQUAL_STRING("/tmp", cwd);

    // Save the current directory
    char old_cwd[CWDBUF];
    getcwd(old_cwd, sizeof(old_cwd));

    // Change to the previous directory
    change_directory("-");
    getcwd(cwd, sizeof(cwd));
    TEST_ASSERT_NOT_EQUAL(old_cwd, cwd);
}

void setUp(void) {
    // Set up the test environment
}

void tearDown(void) {
    // Clean up the test environment
}
