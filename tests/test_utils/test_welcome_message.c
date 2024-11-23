#include "utils.h"
#include "unity.h"
#include <string.h>

/**
 * @brief Test for the print_welcome_message function
 *
 */
void test_print_welcome_message(void);

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_print_welcome_message);
    return UNITY_END();
}

void test_print_welcome_message(void) {
    // Redirect stdout to a buffer
    char buffer[1024];
    FILE* stream = fmemopen(buffer, sizeof(buffer), "w");
    if (stream == NULL) {
        TEST_FAIL_MESSAGE("Failed to open buffer stream");
    }

    // Save the original stdout
    FILE* original_stdout = stdout;
    stdout = stream;

    // Call the print_welcome_message function
    print_welcome_message();

    // Restore the original stdout
    fflush(stream);
    stdout = original_stdout;

    // Check the output
    fclose(stream);

    // Get current date and time
    time_t now = time(NULL);
    struct tm* t = localtime(&now);
    char date_time[100];
    strftime(date_time, sizeof(date_time) - 1, "%Y-%m-%d %H:%M:%S", t);

    // Construct the expected output
    char expected_output[1024];
    snprintf(expected_output, sizeof(expected_output),
             "*********************************************************\n"
             "*        _          _____  _    _  ______ _      _       *\n"
             "*       | |        / ____|| |  | ||  ____| |    | |      *\n"
             "*       | |  ___  | (___  | |__| || |__  | |    | |      *\n"
             "*  _    | | |___|  \\___ \\ |  __  ||  __| | |    | |      *\n"
             "* | \\___| |        ____) || |  | || |____| |____| |____  *\n"
             "*  \\_____/        |_____/ |_|  |_||______|______|______| *\n"
             "*                                                        *\n"
             "*        Welcome to my shell!                            *\n"
             "*        Developed by Jorge Arbach.                      *\n"
             "*        Write 'help' to see available commmands         *\n"
             "*                                                        *\n"
             "* Date and time: %s                     *\n"
             "********************************************************\n", date_time);

    TEST_ASSERT_EQUAL_STRING(expected_output, buffer);
}

void setUp(void) {
    // Set up code if needed
}

void tearDown(void) {
    // Tear down code if needed
}
