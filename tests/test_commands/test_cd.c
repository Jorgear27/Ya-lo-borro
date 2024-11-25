#include "commands.h"
#include "unity.h"

/**
 * @brief Buffer size for the tests
 *
 */
#define CWDBUF 1024

/**
 * @brief Buffer size for the tests
 *
 */
#define BUFFER_SIZE 256

/**
 * @brief Maximum number of arguments
 *
 */
#define MAX_ARGS 64

/**
 * @brief Test the change_directory function
 *
 */
void test_change_directory(void);

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

/**
 * @brief Test for the show_help function
 *
 */
void test_show_help(void);

/**
 * @brief Test for the is_internal_command function
 *
 */
void test_is_internal_command(void);

/**
 * @brief Test for the execute_command function
 *
 */
void test_execute_command(void);

/**
 * @brief Test for the execute_program function
 *
 */
void test_execute_program(void);

/**
 * @brief Test for the execute_piped_commands function
 *
 */
void test_execute_piped_commands(void);

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

/**
 * @brief Test for the parse_pipes function
 *
 */
void test_parse_pipes(void);

/**
 * @brief Test for the quit_shell function
 *
 */
void test_quit_shell(void);

/**
 * @brief Test for the execute_batch_file function
 *
 */
void test_execute_batch_file(void);

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_change_directory);
    RUN_TEST(test_echo_plain_message);
    RUN_TEST(test_echo_quoted_message);
    RUN_TEST(test_show_help);
    RUN_TEST(test_is_internal_command);
    RUN_TEST(test_execute_command);
    RUN_TEST(test_execute_program);
    RUN_TEST(test_execute_piped_commands);
    RUN_TEST(test_parse_input);
    RUN_TEST(test_parse_input_with_input_redirection);
    RUN_TEST(test_parse_pipes);
    RUN_TEST(test_quit_shell);
    RUN_TEST(test_execute_batch_file);
    return UNITY_END();
}

void test_change_directory(void) {
    // Change to /tmp directory
    change_directory("/tmp");

    // Check if the current directory is /tmp
    char cwd[CWDBUF];
    getcwd(cwd, sizeof(cwd));
    TEST_ASSERT_EQUAL_STRING("/tmp", cwd);

    // Save the current directory
    char old_cwd[CWDBUF];
    getcwd(old_cwd, sizeof(old_cwd));

    // Change to the previous directory
    change_directory("-");
    getcwd(cwd, sizeof(cwd));
    TEST_ASSERT_NOT_EQUAL(old_cwd, cwd);
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

void test_show_help(void) {
    // Redirect stdout to a buffer
    char buffer[BUFFER_SIZE];
    FILE* stream = fmemopen(buffer, sizeof(buffer), "w");
    if (stream == NULL) {
        TEST_FAIL_MESSAGE("Failed to open buffer stream");
    }

    // Save the original stdout
    FILE* original_stdout = stdout;
    stdout = stream;

    // Call the show_help function
    show_help();

    // Restore the original stdout
    fflush(stream);
    stdout = original_stdout;

    // Check the output
    fclose(stream);
    const char* expected_output = "Available commands:\n"
                                  "  cd <directory>    - Changes current directoryl\n"
                                  "  clr               - Cleans the screen\n"
                                  "  echo <message>    - Shows a message or an environment variable\n"
                                  "  quit              - Closes the shell\n"
                                  "  help              - Shows this help\n"
                                  "  start_monitor     - Starts the monitoring program\n"
                                  "  stop_monitor      - Stops the monitoring program\n"
                                  "  status_monitor    - Shows the status of the monitoring program\n"
                                  "  show_config       - Shows the current configuration of the monitoring program\n"
                                  "  update_config <key> <value> - Updates the configuration of the monitoring program\n";
    TEST_ASSERT_EQUAL_STRING(expected_output, buffer);
}

void test_is_internal_command(void) {
    char* args1[] = {"cd", NULL};
    char* args2[] = {"invalid", NULL};

    TEST_ASSERT_TRUE(is_internal_command(args1));
    TEST_ASSERT_FALSE(is_internal_command(args2));
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

void test_execute_program(void) {
    int pipe_fds[2];
    if (pipe(pipe_fds) == -1) {
        TEST_FAIL_MESSAGE("Failed to create pipe");
    }

    pid_t pid = fork();
    if (pid == -1) {
        TEST_FAIL_MESSAGE("Failed to fork process");
    } else if (pid == 0) {
        // Child process
        close(pipe_fds[0]); // Close read end of the pipe
        dup2(pipe_fds[1], STDOUT_FILENO); // Redirect stdout to the pipe
        close(pipe_fds[1]); // Close write end of the pipe

        // Execute the program uname which is not a shell built-in
        char* args[] = {"uname", NULL};
        execute_program(args);

        exit(EXIT_SUCCESS);
    } else {
        // Parent process
        close(pipe_fds[1]); // Close write end of the pipe

        // Read the output from the pipe
        char buffer[BUFFER_SIZE];
        ssize_t count = read(pipe_fds[0], buffer, sizeof(buffer) - 1);
        if (count == -1) {
            TEST_FAIL_MESSAGE("Failed to read from pipe");
        }
        buffer[count] = '\0';

        close(pipe_fds[0]); // Close read end of the pipe

        // Wait for the child process to finish
        int status;
        waitpid(pid, &status, 0);
        if (!WIFEXITED(status)) {
            TEST_FAIL_MESSAGE("Child process did not exit normally");
        }

        // Check the output
        TEST_ASSERT_NOT_NULL(strstr(buffer, "Linux"));
    }
}

void test_execute_piped_commands(void) {
    int pipe_fds[2];
    if (pipe(pipe_fds) == -1) {
        TEST_FAIL_MESSAGE("Failed to create pipe");
    }

    pid_t pid = fork();
    if (pid == -1) {
        TEST_FAIL_MESSAGE("Failed to fork process");
    } else if (pid == 0) {
        // Child process
        close(pipe_fds[0]); // Close read end of the pipe
        dup2(pipe_fds[1], STDOUT_FILENO); // Redirect stdout to the pipe
        close(pipe_fds[1]); // Close write end of the pipe

        // Execute the piped commands
        char input[] = "echo Hello, World! | tr '[:lower:]' '[:upper:]'";
        execute_piped_commands(input);

        exit(EXIT_SUCCESS);
    } else {
        // Parent process
        close(pipe_fds[1]); // Close write end of the pipe

        // Read the output from the pipe
        char buffer[BUFFER_SIZE];
        ssize_t count = read(pipe_fds[0], buffer, sizeof(buffer) - 1);
        if (count == -1) {
            TEST_FAIL_MESSAGE("Failed to read from pipe");
        }
        buffer[count] = '\0'; // Null-terminate the buffer

        close(pipe_fds[0]); // Close read end of the pipe

        // Wait for the child process to finish
        int status;
        waitpid(pid, &status, 0);
        if (!WIFEXITED(status)) {
            TEST_FAIL_MESSAGE("Child process did not exit normally");
        }

        // Check the output
        TEST_ASSERT_EQUAL_STRING("HELLO, WORLD!\n", buffer);
    }
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

void test_quit_shell(void) {
    pid_t pid = fork();
    if (pid == -1) {
        TEST_FAIL_MESSAGE("Failed to fork process");
    } else if (pid == 0) {
        // Child process
        quit_shell();
        // If quit_shell() works correctly, the following line should not be executed
        TEST_FAIL_MESSAGE("quit_shell() did not exit the process");
        exit(EXIT_FAILURE);
    } else {
        // Parent process
        int status;
        waitpid(pid, &status, 0);
        if (WIFEXITED(status)) {
            TEST_ASSERT_EQUAL_INT(EXIT_SUCCESS, WEXITSTATUS(status));
        } else {
            TEST_FAIL_MESSAGE("Child process did not exit normally");
        }
    }
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
    // Clear buffers if necessary
    fflush(stdout);
    fflush(stderr);
}

void tearDown(void) {
    // Clear buffers if necessary
    fflush(stdout);
    fflush(stderr);
}
