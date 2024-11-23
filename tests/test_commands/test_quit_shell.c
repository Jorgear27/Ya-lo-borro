#include "commands.h"
#include "unity.h"

/**
 * @brief Test for the quit_shell function
 *
 */
void test_quit_shell(void);

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_quit_shell);
    return UNITY_END();
}

void test_quit_shell(void) {
    pid_t pid = fork();
    if (pid == -1) {
        TEST_FAIL_MESSAGE("Failed to fork process");
    } else if (pid == 0) {
        // Child process
        quit_shell();
        // If quit_shell() works correctly, the following line should not be executed
        TEST_FAIL_MESSAGE("quit_shell() did not exit the process");
        exit(EXIT_FAILURE);
    } else {
        // Parent process
        int status;
        waitpid(pid, &status, 0);
        if (WIFEXITED(status)) {
            TEST_ASSERT_EQUAL_INT(EXIT_SUCCESS, WEXITSTATUS(status));
        } else {
            TEST_FAIL_MESSAGE("Child process did not exit normally");
        }
    }
}

void setUp(void) {
    // Set up code if needed
}

void tearDown(void) {
    // Tear down code if needed
}
