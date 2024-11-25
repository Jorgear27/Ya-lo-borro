#include "monitor_commands.h"
#include "unity.h"
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <ctype.h>

/**
 * @brief Buffer size for the tests
 *
 */
#define BUFFER_SIZE 256

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

/**
 * @brief Test for the stop_monitor function
 *
 */
void test_stop_monitor(void);

/**
 * @brief Test for the is_monitor_command function
 *
 */
void test_is_monitor_command(void);

/**
 * @brief Normalize whitespace in a string
 *
 * @param str The input string
 * @return The normalized string
 */
char* normalize_whitespace(const char* str);

/**
 * @brief Test for the show_configuration function
 *
 */
void test_show_configuration(void);

/**
 * @brief Test for the update_configuration function
 *
 */
void test_update_configuration(void);

/**
 * @brief Test for the update_configuration function with wrong key and value
 *
 */
void test_update_configuration_wrong(void);


/**
 * @brief Test for the status_monitor function
 *
 */
void test_status_monitor(void);

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_start_monitor);
    RUN_TEST(test_start_monitor_already_running);
    RUN_TEST(test_start_monitor_invalid_config);
    RUN_TEST(test_stop_monitor);
    RUN_TEST(test_is_monitor_command);
    RUN_TEST(test_show_configuration);
    RUN_TEST(test_update_configuration);
    RUN_TEST(test_update_configuration_wrong);
    RUN_TEST(test_status_monitor);
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
    }
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

void test_show_configuration(void) {
    // Create a temporary config file in the correct directory
    const char* config_filename = "tmp_config.json";
    FILE* config_file = fopen(config_filename, "w");
    if (!config_file) {
        TEST_FAIL_MESSAGE("Failed to create config file");
    }

    // Write some configuration to the file
    const char* config_content = "{\"sleep_time\": 5, \"metrics\": {\"cpu\": true, \"memory\": true}}";
    fprintf(config_file, "%s", config_content);
    fclose(config_file);

    // Redirect stdout to a buffer
    char buffer[BUFFER_SIZE*4];
    FILE* stream = fmemopen(buffer, sizeof(buffer), "w");
    if (stream == NULL) {
        TEST_FAIL_MESSAGE("Failed to open buffer stream");
    }

    // Save the original stdout
    FILE* original_stdout = stdout;
    stdout = stream;

    // Call the show_configuration function
    show_configuration(config_filename);

    // Restore the original stdout
    fflush(stream);
    stdout = original_stdout;

    // Check the output
    fclose(stream);

    // Construct the expected output
    const char* expected_output = "Current configuration:\n"
                                  "{\n"
                                  "    \"sleep_time\": 5,\n"
                                  "    \"metrics\": {\n"
                                  "        \"cpu\": true,\n"
                                  "        \"memory\": true\n"
                                  "    }\n"
                                  "}\n";

    // Normalize whitespace in both the expected and actual output
    char* normalized_expected = normalize_whitespace(expected_output);
    char* normalized_actual = normalize_whitespace(buffer);

    TEST_ASSERT_EQUAL_STRING(normalized_expected, normalized_actual);

    // Clean up
    unlink(config_filename);
}

void test_update_configuration(void) {
    // Create a temporary config file in the correct directory
    const char* config_filename = "tmp_config.json";
    FILE* config_file = fopen(config_filename, "w");
    if (!config_file) {
        TEST_FAIL_MESSAGE("Failed to create config file");
    }

    // Write initial configuration to the file
    const char* initial_config = "{\"sleep_time\": 5, \"metrics\": {\"cpu\": true, \"memory\": true}}";
    fprintf(config_file, "%s", initial_config);
    fclose(config_file);

    // Update the configuration
    update_configuration("sleep_time", "10", config_filename);
    update_configuration("cpu", "false", config_filename);

    // Read the updated configuration
    config_file = fopen(config_filename, "r");
    if (!config_file) {
        TEST_FAIL_MESSAGE("Failed to open config file");
    }

    char buffer[BUFFER_SIZE*4];
    fread(buffer, 1, BUFFER_SIZE*4, config_file);
    fclose(config_file);

    // Construct the expected updated configuration
    const char* expected_config = "{\"sleep_time\":10,\"metrics\":{\"cpu\":false,\"memory\":true}}";

    // Normalize whitespace in both the expected and actual output
    char* normalized_expected = normalize_whitespace(expected_config);
    char* normalized_actual = normalize_whitespace(buffer);

    TEST_ASSERT_EQUAL_STRING(normalized_expected, normalized_actual);

    // Clean up
    unlink(config_filename);
}

void test_update_configuration_wrong(void) {
    // Create a temporary config file in the correct directory
    const char* config_filename = "tmp_config.json";
    FILE* config_file = fopen(config_filename, "w");
    if (!config_file) {
        TEST_FAIL_MESSAGE("Failed to create config file");
    }

    // Write initial configuration to the file
    const char* initial_config = "{\"sleep_time\": 5, \"metrics\": {\"cpu\": true, \"memory\": true}}";
    fprintf(config_file, "%s", initial_config);
    fclose(config_file);

    // Redirect stdout to a buffer
    char buffer[BUFFER_SIZE];
    FILE* stream = fmemopen(buffer, sizeof(buffer), "w");
    if (stream == NULL) {
        TEST_FAIL_MESSAGE("Failed to open buffer stream");
    }

    // Save the original stdout
    FILE* original_stdout = stdout;
    stdout = stream;

    // Update the configuration
    update_configuration("sleeper_time", "10", config_filename);
    update_configuration("cpu", "10", config_filename);

    // Restore the original stdout
    fflush(stream);
    stdout = original_stdout;
    fclose(stream);

    // Construct the expected wrong key and value messages
    const char* expected_message = "Key not found in configuration file\nInvalid value for key: %s. Only 'true(1)' or 'false(0)' are allowed.\n";

    // Normalize whitespace in both the expected and actual output
    char* expected = normalize_whitespace(expected_message);
    char* actual = normalize_whitespace(buffer);

    // Check the output
    TEST_ASSERT_EQUAL_STRING(expected, actual);

    // Clean up
    unlink(config_filename);
}

void test_status_monitor(void) {
    // Redirect stdout to a buffer
    char buffer[BUFFER_SIZE];
    FILE* stream = fmemopen(buffer, sizeof(buffer), "w");
    if (stream == NULL) {
        TEST_FAIL_MESSAGE("Failed to open buffer stream");
    }

    // Save the original stdout
    FILE* original_stdout = stdout;
    stdout = stream;

    // Call the status_monitor function when monitor is not running
    status_monitor();

    // Restore the original stdout
    fflush(stream);
    stdout = original_stdout;

    // Check the output
    fclose(stream);
    TEST_ASSERT_EQUAL_STRING("Monitor is not running.\n", buffer);

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

        // Redirect stdout to a buffer again
        stream = fmemopen(buffer, sizeof(buffer), "w");
        if (stream == NULL) {
            TEST_FAIL_MESSAGE("Failed to open buffer stream");
        }

        // Save the original stdout
        original_stdout = stdout;
        stdout = stream;

        // Call the status_monitor function when monitor is running
        status_monitor();

        // Restore the original stdout
        fflush(stream);
        stdout = original_stdout;

        // Check the output
        fclose(stream);
        char expected_output[BUFFER_SIZE];
        snprintf(expected_output, sizeof(expected_output), "Monitor is running with PID %d.\n", monitor_pid);
        TEST_ASSERT_EQUAL_STRING(expected_output, buffer);

        // Clean up
        kill(monitor_pid, SIGTERM);
        waitpid(monitor_pid, NULL, 0);
        monitor_pid = -1;
    }
}

char* normalize_whitespace(const char* str) {
    static char normalized[BUFFER_SIZE*4];
    char* dest = normalized;
    const char* src = str;
    while (*src) {
        if (isspace((unsigned char)*src)) {
            *dest++ = ' ';
            while (isspace((unsigned char)*src)) {
                src++;
            }
        } else {
            *dest++ = *src++;
        }
    }
    *dest = '\0';
    return normalized;
}

void setUp(void) {
    // Ensure the monitor is not running before each test
    stop_monitor();
    monitor_pid = -1;
}

void tearDown(void) {
    // Ensure the monitor is not running after each test
    stop_monitor();

    // Remove the pipe
    if (monitor_pid != -1) {
        kill(monitor_pid, SIGTERM);
        waitpid(monitor_pid, NULL, 0);
        monitor_pid = -1;
    }
    unlink(PIPE_PATH);
}
