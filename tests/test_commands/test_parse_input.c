#include "commands.h"
#include "unity.h"

/**
 * @brief Maximum number of arguments
 *
 */
#define MAX_ARGS 64

/**
 * @brief Test for the parse_input function
 *
 */
void test_parse_input(void);

/**
 * @brief Test for the parse_input function with input redirection
 *
 */
void test_parse_input_with_input_redirection(void);

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_parse_input);
    RUN_TEST(test_parse_input_with_input_redirection);
    return UNITY_END();
}

void test_parse_input(void) {
    char input[] = "echo Hello, World! > output.txt";
    char* args[MAX_ARGS];
    char* input_file = NULL;
    char* output_file = NULL;

    parse_input(input, args, &input_file, &output_file);

    // Check the parsed arguments
    TEST_ASSERT_EQUAL_STRING("echo", args[0]);
    TEST_ASSERT_EQUAL_STRING("Hello,", args[1]);
    TEST_ASSERT_EQUAL_STRING("World!", args[2]);
    TEST_ASSERT_NULL(args[3]);

    // Check the output file redirection
    TEST_ASSERT_EQUAL_STRING("output.txt", output_file);
    TEST_ASSERT_NULL(input_file);
}

void test_parse_input_with_input_redirection(void) {
    char input[] = "sort < input.txt";
    char* args[MAX_ARGS];
    char* input_file = NULL;
    char* output_file = NULL;

    parse_input(input, args, &input_file, &output_file);

    // Check the parsed arguments
    TEST_ASSERT_EQUAL_STRING("sort", args[0]);
    TEST_ASSERT_NULL(args[1]);

    // Check the input file redirection
    TEST_ASSERT_EQUAL_STRING("input.txt", input_file);
    TEST_ASSERT_NULL(output_file);
}

void setUp(void) {
    // Set up code if needed
}

void tearDown(void) {
    // Tear down code if needed
}
