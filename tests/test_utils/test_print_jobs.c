#include "utils.h"
#include "unity.h"
#include <string.h>

/**
 * @brief Test for the print_completed_jobs function
 *
 */
void test_print_completed_jobs(void);

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_print_completed_jobs);
    return UNITY_END();
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
    char buffer[256];
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
