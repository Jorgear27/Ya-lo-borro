#include "monitor_commands.h"
#include "unity.h"

/**
 * @brief Test for the is_monitor_command function
 *
 */
void test_is_monitor_command(void);

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_is_monitor_command);
    return UNITY_END();
}

void test_is_monitor_command(void) {
    char* valid_commands[][2] = {
        {"start_monitor", NULL},
        {"stop_monitor", NULL},
        {"status_monitor", NULL},
        {"update_config", "key value"},
        {"show_config", NULL},
        {NULL, NULL}
    };

    char* invalid_commands[][2] = {
        {"invalid_command", NULL},
        {"echo", "Hello, World!"},
        {"cd", "/home"},
        {NULL, NULL}
    };

    // Test valid monitor commands
    for (int i = 0; valid_commands[i][0] != NULL; i++) {
        TEST_ASSERT_TRUE(is_monitor_command(valid_commands[i]));
    }

    // Test invalid monitor commands
    for (int i = 0; invalid_commands[i][0] != NULL; i++) {
        TEST_ASSERT_FALSE(is_monitor_command(invalid_commands[i]));
    }
}

void setUp(void) {
    // Set up code if needed
}

void tearDown(void) {
    // Tear down code if needed
}
