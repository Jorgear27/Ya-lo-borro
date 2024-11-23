#include "monitor_commands.h"
#include "unity.h"

/**
 * @brief Buffer size for the tests
 *
 */
#define BUFFER_SIZE 256

/**
 * @brief Test for the stop_monitor function
 *
 */
void test_stop_monitor(void);

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_stop_monitor);
    return UNITY_END();
}

void test_stop_monitor(void) {
    // Start the monitor process
    monitor_pid = fork();
    if (monitor_pid == -1) {
        TEST_FAIL_MESSAGE("Failed to fork process");
    } else if (monitor_pid == 0) {
        // Child process
        while (1) {
            sleep(1); // Simulate a running monitor process
        }
        exit(EXIT_SUCCESS);
    } else {
        // Parent process
        usleep(10000); // Wait for the monitor to start

        // Redirect stdout to a buffer
        char buffer[BUFFER_SIZE];
        FILE* stream = fmemopen(buffer, sizeof(buffer), "w");
        if (stream == NULL) {
            TEST_FAIL_MESSAGE("Failed to open buffer stream");
        }

        // Save the original stdout
        FILE* original_stdout = stdout;
        stdout = stream;

        // Call the stop_monitor function
        stop_monitor();

        // Restore the original stdout
        fflush(stream);
        stdout = original_stdout;

        // Check the output
        fclose(stream);
        TEST_ASSERT_NOT_NULL(strstr(buffer, "Monitor stopped."));

        // Check if the monitor process is stopped
        int status;
        pid_t result = waitpid(monitor_pid, &status, WNOHANG);
        TEST_ASSERT_EQUAL(monitor_pid, result);
        TEST_ASSERT_TRUE(WIFEXITED(status) || WIFSIGNALED(status));

        // Reset monitor_pid
        monitor_pid = -1;
    }
}

void setUp(void) {
    monitor_pid = -1;
}

void tearDown(void) {
    if (monitor_pid != -1) {
        kill(monitor_pid, SIGTERM);
        waitpid(monitor_pid, NULL, 0);
        monitor_pid = -1;
    }
    unlink(PIPE_PATH);
}
