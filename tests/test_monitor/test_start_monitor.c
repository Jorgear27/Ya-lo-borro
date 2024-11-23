#include "monitor_commands.h"
#include "unity.h"
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>

/**
 * @brief Test the start_monitor function.
 *
 */
void test_start_monitor(void);

/**
 * @brief Test the start_monitor function when the monitor is already running.
 *
 */
void test_start_monitor_already_running(void);

/**
 * @brief Test the start_monitor function with an invalid configuration file.
 *
 */
void test_start_monitor_invalid_config(void);

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_start_monitor);
    RUN_TEST(test_start_monitor_already_running);
    RUN_TEST(test_start_monitor_invalid_config);
    return UNITY_END();
}

void test_start_monitor(void) {
    start_monitor(CONFIG_FILE);
    TEST_ASSERT_NOT_EQUAL(-1, monitor_pid);
    status_monitor();
}

void test_start_monitor_already_running(void) {
    start_monitor(CONFIG_FILE);
    pid_t first_pid = monitor_pid;
    start_monitor(CONFIG_FILE);
    TEST_ASSERT_EQUAL(first_pid, monitor_pid);
    status_monitor();
}

void test_start_monitor_invalid_config(void) {
    const char* invalid_config = "/invalid/path/config.json";
    start_monitor(invalid_config);
    TEST_ASSERT_EQUAL(-1, monitor_pid);
    status_monitor();
}

void setUp(void) {
    // Ensure the monitor is not running before each test
    stop_monitor();
}

void tearDown(void) {
    // Ensure the monitor is not running after each test
    stop_monitor();
}
