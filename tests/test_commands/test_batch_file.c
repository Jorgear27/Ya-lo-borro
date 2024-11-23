#include "commands.h"
#include "unity.h"

/**
 * @brief Buffer size for the tests
 *
 */
#define BUFFER_SIZE 256

/**
 * @brief Test for the execute_batch_file function
 *
 */
void test_execute_batch_file(void);

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_execute_batch_file);
    return UNITY_END();
}

void test_execute_batch_file(void) {
    // Create a temporary batch file
    const char* filename = "test_batch_file.txt";
    FILE* file = fopen(filename, "w");
    if (!file) {
        TEST_FAIL_MESSAGE("Failed to create batch file");
    }

    // Write commands to the batch file
    fprintf(file, "echo Listing files in the current directory...\n");
    fprintf(file, "ls -l > ls_output.txt\n");
    fprintf(file, "echo Finding all .txt files in the current directory...\n");
    fprintf(file, "ls -l | grep .txt > txt_files.txt\n");
    fprintf(file, "echo Sorting the contents of txt_files.txt...\n");
    fprintf(file, "sort txt_files.txt > sorted_txt_files.txt\n");

    fclose(file);

    // Redirect stdout to a buffer
    char buffer[BUFFER_SIZE];
    FILE* stream = fmemopen(buffer, sizeof(buffer), "w");
    if (stream == NULL) {
        TEST_FAIL_MESSAGE("Failed to open buffer stream");
    }

    // Save the original stdout
    FILE* original_stdout = stdout;
    stdout = stream;

    // Execute the batch file
    execute_batch_file(filename);

    // Restore the original stdout
    fflush(stream);
    stdout = original_stdout;

    // Check the output
    fclose(stream);
    const char* expected_output = "Listing files in the current directory...\n"
                                  "Finding all .txt files in the current directory...\n"
                                  "Sorting the contents of txt_files.txt...\n";
    TEST_ASSERT_EQUAL_STRING(expected_output, buffer);

    // Check the creation of ls_output.txt
    FILE* ls_output_file = fopen("ls_output.txt", "r");
    TEST_ASSERT_NOT_NULL(ls_output_file);

    // Check the creation of txt_files.txt
    FILE* txt_files_file = fopen("txt_files.txt", "r");
    TEST_ASSERT_NOT_NULL(txt_files_file);

    // Check the creation of sorted_txt_files.txt
    FILE* sorted_txt_files_file = fopen("sorted_txt_files.txt", "r");
    TEST_ASSERT_NOT_NULL(sorted_txt_files_file);

    // Clean up
    remove(filename);
    remove("ls_output.txt");
    remove("txt_files.txt");
    remove("sorted_txt_files.txt");
}

void setUp(void) {
    // Set up code if needed
}

void tearDown(void) {
    // Tear down code if needed
}
