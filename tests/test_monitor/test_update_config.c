#include "monitor_commands.h"
#include "unity.h"
#include <ctype.h>

/**
 * @brief Buffer size for the tests
 *
 */
#define BUFFER_SIZE 1024

/**
 * @brief Test for the update_configuration function
 *
 */
void test_update_configuration(void);

/**
 * @brief Normalize whitespace in a string
 *
 * @param str The input string
 * @return The normalized string
 */
char* normalize_whitespace(const char* str);

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_update_configuration);
    return UNITY_END();
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

    char buffer[BUFFER_SIZE];
    fread(buffer, 1, BUFFER_SIZE, config_file);
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

char* normalize_whitespace(const char* str) {
    static char normalized[BUFFER_SIZE];
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
    // Set up code if needed
}

void tearDown(void) {
    // Tear down code if needed
}
