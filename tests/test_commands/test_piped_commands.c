#include "commands.h"
#include "unity.h"

/**
 * @brief Buffer size for the tests
 *
 */
#define BUFFER_SIZE 256

/**
 * @brief Test for the execute_piped_commands function
 *
 */
void test_execute_piped_commands(void);

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_execute_piped_commands);
    return UNITY_END();
}

void test_execute_piped_commands(void) {
    int pipe_fds[2];
    if (pipe(pipe_fds) == -1) {
        TEST_FAIL_MESSAGE("Failed to create pipe");
    }

    pid_t pid = fork();
    if (pid == -1) {
        TEST_FAIL_MESSAGE("Failed to fork process");
    } else if (pid == 0) {
        // Child process
        close(pipe_fds[0]); // Close read end of the pipe
        dup2(pipe_fds[1], STDOUT_FILENO); // Redirect stdout to the pipe
        close(pipe_fds[1]); // Close write end of the pipe

        // Execute the piped commands
        char input[] = "echo Hello, World! | tr '[:lower:]' '[:upper:]'";
        execute_piped_commands(input);

        exit(EXIT_SUCCESS);
    } else {
        // Parent process
        close(pipe_fds[1]); // Close write end of the pipe

        // Read the output from the pipe
        char buffer[BUFFER_SIZE];
        ssize_t count = read(pipe_fds[0], buffer, sizeof(buffer) - 1);
        if (count == -1) {
            TEST_FAIL_MESSAGE("Failed to read from pipe");
        }
        buffer[count] = '\0'; // Null-terminate the buffer

        close(pipe_fds[0]); // Close read end of the pipe

        // Wait for the child process to finish
        int status;
        waitpid(pid, &status, 0);
        if (!WIFEXITED(status)) {
            TEST_FAIL_MESSAGE("Child process did not exit normally");
        }

        // Check the output
        TEST_ASSERT_EQUAL_STRING("HELLO, WORLD!\n", buffer);
    }
}

void setUp(void) {
    // Set up code if needed
}

void tearDown(void) {
    // Tear down code if needed
}
