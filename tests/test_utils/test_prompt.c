#include "utils.h"
#include "unity.h"
#include <string.h>

/**
 * @brief Test for the display_prompt function
 *
 */
void test_display_prompt(void);

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_display_prompt);
    return UNITY_END();
}

void test_display_prompt(void) {
    // Redirect stdout to a buffer
    int stdout_fd = dup(STDOUT_FILENO);
    int pipe_fds[2];
    pipe(pipe_fds);
    dup2(pipe_fds[1], STDOUT_FILENO);
    close(pipe_fds[1]);

    // Call display_prompt
    display_prompt();
    char buffer[COMMAND_MAX];
    read(pipe_fds[0], buffer, sizeof(buffer) - 1);
    buffer[COMMAND_MAX-1] = '\0';

    // Restore stdout
    dup2(stdout_fd, STDOUT_FILENO);
    close(stdout_fd);

    // Check if the prompt was displayed
    TEST_ASSERT_TRUE(strstr(buffer, getenv("USER")) != NULL);
    TEST_ASSERT_TRUE(strstr(buffer, "@") != NULL);
    TEST_ASSERT_TRUE(strstr(buffer, ":") != NULL);
    TEST_ASSERT_TRUE(strstr(buffer, "$") != NULL);
}

void setUp(void) {
    // Set up code if needed
}

void tearDown(void) {
    // Tear down code if needed
}
