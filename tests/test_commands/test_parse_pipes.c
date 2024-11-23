#include "commands.h"
#include "unity.h"

/**
 * @brief Maximum number of arguments
 *
 */
#define MAX_ARGS 64

/**
 * @brief Test for the parse_pipes function
 *
 */
void test_parse_pipes(void);

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_parse_pipes);
    return UNITY_END();
}

void test_parse_pipes(void) {
    char input[] = "echo Hello, World! | tr '[:lower:]' '[:upper:]' | wc -c";
    char* commands[MAX_ARGS];

    int num_commands = parse_pipes(input, commands);

    // Check the number of parsed commands
    TEST_ASSERT_EQUAL_INT(3, num_commands);

    // Check the parsed commands
    TEST_ASSERT_EQUAL_STRING("echo Hello, World!", commands[0]);
    TEST_ASSERT_EQUAL_STRING("tr '[:lower:]' '[:upper:]'", commands[1]);
    TEST_ASSERT_EQUAL_STRING("wc -c", commands[2]);
    TEST_ASSERT_NULL(commands[3]);
}

void setUp(void) {
    // Set up code if needed
}

void tearDown(void) {
    // Tear down code if needed
}
