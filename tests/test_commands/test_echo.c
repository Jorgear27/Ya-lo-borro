#include "commands.h"
#include "unity.h"

/**
 * @brief Buffer size for the tests
 *
 */
#define BUFFER_SIZE 256

/**
 * @brief Test for the echo function
 *
 */
void test_echo_plain_message(void);

/**
 * @brief Test for the echo function with a quoted message
 *
 */
void test_echo_quoted_message(void);

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_echo_plain_message);
    RUN_TEST(test_echo_quoted_message);
    return UNITY_END();
}

void test_echo_plain_message(void) {
    char* message = "Hello, World!";

    // Redirect stdout to a buffer
    char buffer[BUFFER_SIZE];
    FILE* stream = fmemopen(buffer, sizeof(buffer), "w");
    if (stream == NULL) {
        TEST_FAIL_MESSAGE("Failed to open buffer stream");
    }

    // Save the original stdout
    FILE* original_stdout = stdout;
    stdout = stream;

    // Call the echo function
    echo(message);

    // Restore the original stdout
    fflush(stream);
    stdout = original_stdout;

    // Check the output
    fclose(stream);
    TEST_ASSERT_EQUAL_STRING("Hello, World!\n", buffer);
}

void test_echo_quoted_message(void) {
    char* message = "\"Hello, World!\"";

    // Redirect stdout to a buffer
    char buffer[BUFFER_SIZE];
    FILE* stream = fmemopen(buffer, sizeof(buffer), "w");
    if (stream == NULL) {
        TEST_FAIL_MESSAGE("Failed to open buffer stream");
    }

    // Save the original stdout
    FILE* original_stdout = stdout;
    stdout = stream;

    // Call the echo function
    echo(message);

    // Restore the original stdout
    fflush(stream);
    stdout = original_stdout;

    // Check the output
    fclose(stream);
    TEST_ASSERT_EQUAL_STRING("Hello, World!\n", buffer);
}

void setUp(void) {
    // Set up code if needed
}

void tearDown(void) {
    // Tear down code if needed
}
