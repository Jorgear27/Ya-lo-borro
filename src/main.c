#include "commands.h"
#include "utils.h"
#include <cjson/cJSON.h>

int main(int argc, char* argv[])
{
    // Register signal handlers
    signal(SIGCHLD, signal_handler);
    signal(SIGINT, signal_handler);
    signal(SIGTSTP, signal_handler);
    signal(SIGQUIT, signal_handler);

    print_welcome_message();

    // Batch mode: execute the commands in the batch file
    if (argc > 1)
    {
        execute_batch_file(argv[1]);
        return 0;
    }

    char* input_file;
    char* output_file;
    char input[COMMAND_MAX];
    char* args[MAX_ARGS];

    // Interactive mode: read and execute commands from the user
    while (true)
    {
        // Show the prompt
        display_prompt();

        // Read the input from the user
        if (fgets(input, sizeof(input), stdin) == NULL)
        {
            perror("Error al leer la entrada");
            continue;
        }

        // Check if the command should be run in the background or foreground
        bool background = false;
        if (input[strlen(input) - 2] == '&')
        {
            printf("Background process\n");
            background = true;
            input[strlen(input) - 2] = '\0'; // Remove the '&' character
        }

        // Execute the command in the foreground or background
        if (background)
        {
            if (job_counter >= MAX_JOBS)
            {
                fprintf(stderr, "Error: Maximum number of background jobs reached.\n");
                continue;
            }
            pid_t pid = fork();
            if (pid == -1)
            {
                perror("Error en fork");
            }
            else if (pid == 0)
            { // Child process
                // Check if the input is a pipe
                if (strchr(input, '|') != NULL)
                {
                    // If the input contains a pipe, execute the piped commands
                    execute_piped_commands(input);
                }
                else
                {
                    // Parse the input into arguments
                    parse_input(input, args, &input_file, &output_file);
                    // Execute the command
                    execute_command(args, input_file, output_file);
                }
                exit(EXIT_SUCCESS); // Ensure the child process ends
            }
            else
            { // Father process
                job_counter++;
                background_jobs[job_counter - 1].job_id = job_counter;
                background_jobs[job_counter - 1].pid = pid;
                strncpy(background_jobs[job_counter - 1].command, input,
                        sizeof(background_jobs[job_counter - 1].command) - 1);
                background_jobs[job_counter - 1].command[sizeof(background_jobs[job_counter - 1].command) - 1] = '\0';
                printf("[%d] %d\n", job_counter, pid);
                usleep(100); // Short pause to ensure the child prints before the prompt
            }
        }
        else
        {
            if (strchr(input, '|') != NULL)
            {
                // If the input contains a pipe, execute the piped commands
                execute_piped_commands(input);
            }
            else
            {
                // Parse the input into arguments
                parse_input(input, args, &input_file, &output_file);
                // Execute the command
                execute_command(args, input_file, output_file);
            }
        }

        // Wait for the foreground process to end
        if (!background && foreground_pid > 0)
        {
            waitpid(foreground_pid, NULL, 0); // Wait for the foreground process
            foreground_pid = -1;              // Reset after the process ends
        }

        // Print completed background jobs if any
        if (background_job_completed)
        {
            print_completed_jobs();
            background_job_completed = 0; // Reset the flag
        }
    }
    return 0;
}
