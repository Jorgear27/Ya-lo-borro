#include "commands.h"
#include "unity.h"

/**
 * @brief Buffer size for the tests
 *
 */
#define BUFFER_SIZE 1024

/**
 * @brief Test for the show_help function
 *
 */
void test_show_help(void);

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_show_help);
    return UNITY_END();
}

void test_show_help(void) {
    // Redirect stdout to a buffer
    char buffer[BUFFER_SIZE];
    freopen("/dev/null", "a", stdout);
    setbuf(stdout, buffer);

    // Call the show_help function
    show_help();

    // Restore stdout
    freopen("/dev/tty", "a", stdout);

    // Check the output
    const char* expected_output = "Available commands:\n"
                                  "  cd <directory>    - Changes current directoryl\n"
                                  "  clr               - Cleans the screen\n"
                                  "  echo <message>    - Shows a message or an environment variable\n"
                                  "  quit              - Closes the shell\n"
                                  "  help              - Shows this help\n"
                                  "  start_monitor     - Starts the monitoring program\n"
                                  "  stop_monitor      - Stops the monitoring program\n"
                                  "  status_monitor    - Shows the status of the monitoring program\n"
                                  "  show_config       - Shows the current configuration of the monitoring program\n"
                                  "  update_config <key> <value> - Updates the configuration of the monitoring program\n";
    TEST_ASSERT_EQUAL_STRING(expected_output, buffer);
}

void setUp(void) {
    // Set up code if needed
}

void tearDown(void) {
    // Tear down code if needed
}
