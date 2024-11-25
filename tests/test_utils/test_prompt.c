#include "utils.h"
#include "unity.h"
#include <string.h>

/**
 * @brief Buffer size for the tests
 *
 */
#define BUFFER_SIZE 256

/**
 * @brief Time size for the tests
 *
 */
#define TIME_SIZE 100

/**
 * @brief Test for the display_prompt function
 *
 */
void test_display_prompt(void);

/**
 * @brief Test for the print_completed_jobs function
 *
 */
void test_print_completed_jobs(void);

/**
 * @brief Test for the print_welcome_message function
 *
 */
void test_print_welcome_message(void);

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_display_prompt);
    RUN_TEST(test_print_completed_jobs);
    RUN_TEST(test_print_welcome_message);
    return UNITY_END();
}

void test_display_prompt(void) {
    // Redirect stdout to a buffer
    int stdout_fd = dup(STDOUT_FILENO);
    int pipe_fds[2];
    pipe(pipe_fds);
    dup2(pipe_fds[1], STDOUT_FILENO);
    close(pipe_fds[1]);

    // Call display_prompt
    display_prompt();
    char buffer[COMMAND_MAX];
    read(pipe_fds[0], buffer, sizeof(buffer) - 1);
    buffer[COMMAND_MAX-1] = '\0';

    // Restore stdout
    dup2(stdout_fd, STDOUT_FILENO);
    close(stdout_fd);

    // Check if the prompt was displayed
    TEST_ASSERT_TRUE(strstr(buffer, getenv("USER")) != NULL);
    TEST_ASSERT_TRUE(strstr(buffer, "@") != NULL);
    TEST_ASSERT_TRUE(strstr(buffer, ":") != NULL);
    TEST_ASSERT_TRUE(strstr(buffer, "$") != NULL);
}

void test_print_completed_jobs(void) {
    // Add some background jobs
    background_jobs[0].pid = -1; // Mark as completed
    background_jobs[0].job_id = 1;
    strcpy(background_jobs[0].command, "sleep 10");

    background_jobs[1].pid = -1; // Mark as completed
    background_jobs[1].job_id = 2;
    strcpy(background_jobs[1].command, "ls -l");

    background_jobs[2].pid = 1234; // Not completed
    background_jobs[2].job_id = 3;
    strcpy(background_jobs[2].command, "echo Hello");

    job_counter = 3;
    most_recent_job = 2;
    second_most_recent_job = 1;

    // Redirect stdout to a buffer
    char buffer[BUFFER_SIZE];
    FILE* stream = fmemopen(buffer, sizeof(buffer), "w");
    if (stream == NULL) {
        TEST_FAIL_MESSAGE("Failed to open buffer stream");
    }

    // Save the original stdout
    FILE* original_stdout = stdout;
    stdout = stream;

    // Call the print_completed_jobs function
    print_completed_jobs();

    // Restore the original stdout
    fflush(stream);
    stdout = original_stdout;

    // Check the output
    fclose(stream);
    const char* expected_output = "[1]-  Done                    sleep 10\n"
                                  "[2]+  Done                    ls -l\n";
    TEST_ASSERT_EQUAL_STRING(expected_output, buffer);

    // Check the job counter
    TEST_ASSERT_EQUAL_INT(1, job_counter);
    TEST_ASSERT_EQUAL_INT(3, background_jobs[0].job_id);
}

void test_print_welcome_message(void) {
    // Redirect stdout to a buffer
    char buffer[BUFFER_SIZE*4];
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
    char date_time[TIME_SIZE];
    strftime(date_time, sizeof(date_time) - 1, "%Y-%m-%d %H:%M:%S", t);

    // Construct the expected output
    char expected_output[BUFFER_SIZE*4];
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
    job_counter = 0;
    most_recent_job = -1;
    second_most_recent_job = -1;
    background_job_completed = 0;
    memset(background_jobs, 0, sizeof(background_jobs));
}

void tearDown(void) {
    // Tear down code if needed
}
