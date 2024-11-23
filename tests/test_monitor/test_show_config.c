#include "monitor_commands.h"
#include "unity.h"
#include <ctype.h>

/**
 * @brief Buffer size for the tests
 *
 */
#define BUFFER_SIZE 1024

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

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_show_configuration);
    return UNITY_END();
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
    char buffer[BUFFER_SIZE];
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
