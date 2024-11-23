#include "commands.h"
#include "unity.h"

/**
 * @brief Buffer size for the tests
 *
 */
#define BUFFER_SIZE 256

/**
 * @brief Test for the execute_command function
 *
 */
void test_execute_command(void);

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_execute_command);
    return UNITY_END();
}

void test_execute_command(void) {
    // Redirect stdout to a buffer
    char buffer[BUFFER_SIZE];
    FILE* stream = fmemopen(buffer, sizeof(buffer), "w");
    if (stream == NULL) {
        TEST_FAIL_MESSAGE("Failed to open buffer stream");
    }

    // Save the original stdout
    FILE* original_stdout = stdout;
    stdout = stream;

    char* args[] = {"echo", "Hello, World!", NULL};
    execute_command(args, NULL, NULL);

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
