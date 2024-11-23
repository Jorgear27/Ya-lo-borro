#include "commands.h"
#include "utils.h"

// Commands supported by the shell
char* internal_commands[] = {"cd", "clr", "echo", "quit", "help", NULL};

void change_directory(const char* path)
{
    char* old_pwd = getenv("PWD");

    if (path == NULL)
    { // If no directory is specified, change to the home directory
        printf("%s\n", getenv("PWD"));
    }
    else if (strcmp(path, "-") == 0)
    { // Change to the previous directory
        char* prev_dir = getenv("OLDPWD");
        if (prev_dir)
        {
            chdir(prev_dir);
            setenv("OLDPWD", old_pwd, 1);
            setenv("PWD", prev_dir, 1);
            printf("%s\n", prev_dir); // Show the new directory
        }
        else
        {
            printf("OLDPWD is not established\n");
        }
    }
    else
    { // Change to the specified directory
        if (chdir(path) == 0)
        {
            setenv("OLDPWD", old_pwd, 1);
            setenv("PWD", path, 1);
        }
        else
        {
            perror("Directory not found");
        }
    }
}

void clear_screen(void)
{
    printf("\033[H\033[J"); // ANSI escape code to clear the screen
}

void echo(const char* message)
{
    // Verifies if the message is enclosed in double quotes
    if (message[0] == '"' && message[strlen(message) - 1] == '"')
    {
        // Extracts the message without quotes
        char* trimmed_message = strndup(message + 1, strlen(message) - 2);
        if (trimmed_message)
        {
            printf("%s\n", trimmed_message);
            free(trimmed_message);
        }
    }
    else if (message[0] == '$')
    { // Check if the message is an environment variable
        char* env_var = getenv(message + 1);
        if (env_var)
        {
            printf("%s\n", env_var);
        }
        else
        {
            printf("Environment variable not found\n");
        }
    }
    else
    {
        printf("%s\n", message);
    }
}

void quit_shell(void)
{
    stop_monitor();
    exit(0);
}

void show_help(void)
{
    printf("Available commands:\n");
    printf("  cd <directory>    - Changes current directoryl\n");
    printf("  clr               - Cleans the screen\n");
    printf("  echo <message>    - Shows a message or an environment variable\n");
    printf("  quit              - Closes the shell\n");
    printf("  help              - Shows this help\n");
    printf("  start_monitor     - Starts the monitoring program\n");
    printf("  stop_monitor      - Stops the monitoring program\n");
    printf("  status_monitor    - Shows the status of the monitoring program\n");
    printf("  show_config       - Shows the current configuration of the monitoring program\n");
    printf("  update_config <key> <value> - Updates the configuration of the monitoring program\n");
}

bool is_internal_command(char** args)
{
    for (int i = 0; internal_commands[i] != NULL; i++)
    {
        if (strcmp(args[0], internal_commands[i]) == 0)
        {
            return true;
        }
    }
    return false;
}

void execute_command(char** args, char* input_file, char* output_file)
{
    // Check if it is an empty command
    if (args[0] == NULL)
        return;

    // Save the current stdin and stdout
    int saved_stdin = dup(STDIN_FILENO);
    int saved_stdout = dup(STDOUT_FILENO);

    // Input redirection
    if (input_file)
    {
        int fd_in = open(input_file, O_RDONLY);
        if (fd_in == -1)
        {
            perror("Error al abrir el archivo de entrada");
            exit(EXIT_FAILURE);
        }
        dup2(fd_in, STDIN_FILENO);
        close(fd_in);
    }

    // Output redirection
    if (output_file)
    {
        int fd_out = open(output_file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
        if (fd_out == -1)
        {
            perror("Error al abrir el archivo de salida");
            exit(EXIT_FAILURE);
        }
        dup2(fd_out, STDOUT_FILENO);
        close(fd_out);
    }

    // Execute the command
    if (is_internal_command(args))
    {
        if (strcmp(args[0], "cd") == 0)
        {
            change_directory(args[1]);
        }
        else if (strcmp(args[0], "clr") == 0)
        {
            clear_screen();
        }
        else if (strcmp(args[0], "echo") == 0)
        {
            // Concatenate the arguments to form the message
            char message[COMMAND_MAX] = "";
            for (int i = 1; args[i] != NULL; i++)
            {
                strcat(message, args[i]);
                if (args[i + 1] != NULL)
                {
                    strcat(message, " ");
                }
            }
            echo(message);
        }
        else if (strcmp(args[0], "quit") == 0)
        {
            quit_shell();
        }
        else if (strcmp(args[0], "help") == 0)
        {
            show_help();
        }
    }
    else if (is_monitor_command(args))
    {
        if (strcmp(args[0], "start_monitor") == 0)
        {
            start_monitor(CONFIG_FILE);
        }
        else if (strcmp(args[0], "stop_monitor") == 0)
        {
            stop_monitor();
        }
        else if (strcmp(args[0], "status_monitor") == 0)
        {
            status_monitor();
        }
        else if (strcmp(args[0], "show_config") == 0)
        {
            show_configuration(CONFIG_FILE);
        }
        else if (strcmp(args[0], "update_config") == 0)
        {
            if (args[1] != NULL && args[2] != NULL)
            {
                update_configuration(args[1], args[2], CONFIG_FILE);
            }
            else
            {
                printf("Usage: update_config <key> <value>\n");
            }
        }
    }
    else
    {
        execute_program(args);
    }

    // Restore the stdin and stdout
    dup2(saved_stdin, STDIN_FILENO);
    dup2(saved_stdout, STDOUT_FILENO);

    // Close the saved file descriptors
    close(saved_stdin);
    close(saved_stdout);
}

void execute_program(char** args)
{
    pid_t pid = fork(); // Create a new process

    if (pid < 0)
    {
        // Error creating the process
        perror("Error creating the process");
    }
    else if (pid == 0)
    {
        // Proceso hijo
        execvp(args[0], args); // Execute the command
        // If execvp returns, an error occurred
        printf("Error executing the command\n");
        exit(EXIT_FAILURE);
    }
    else
    {
        // Father process
        int status;
        waitpid(pid, &status, 0);
        if (!WIFEXITED(status))
        {
            printf("Program interrupted\n");
        }
    }
}

void execute_piped_commands(char* input)
{
    char* commands[MAX_ARGS];
    int num_commands = parse_pipes(input, commands); // Divides the input into commands

    int pipe_fds[2];
    int prev_fd = -1; // File descriptor of the previous pipe

    for (int i = 0; i < num_commands; i++)
    {
        pipe(pipe_fds); // Create a pipe

        pid_t pid = fork();

        if (pid == 0)
        {
            // Father process
            if (prev_fd != -1)
            {
                dup2(prev_fd, STDIN_FILENO); // Redirect the input from the previous command
                close(prev_fd);              // Close the previous pipe
            }

            if (i < num_commands - 1)
            {
                dup2(pipe_fds[1], STDOUT_FILENO); // Redirect the output to the next command
            }

            close(pipe_fds[0]); // Close the read end of the pipe
            close(pipe_fds[1]); // Close the write end of the pipe

            // Execute the command
            char* args[MAX_ARGS];
            char* input_file = NULL;
            char* output_file = NULL;
            parse_input(commands[i], args, &input_file, &output_file); // Divide the command into arguments

            // Input redirection
            if (input_file)
            {
                int fd_in = open(input_file, O_RDONLY);
                if (fd_in == -1)
                {
                    perror("Error al abrir el archivo de entrada");
                    exit(EXIT_FAILURE);
                }
                dup2(fd_in, STDIN_FILENO);
                close(fd_in);
            }

            // Output redirection
            if (output_file)
            {
                int fd_out = open(output_file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
                if (fd_out == -1)
                {
                    perror("Error al abrir el archivo de salida");
                    exit(EXIT_FAILURE);
                }
                dup2(fd_out, STDOUT_FILENO);
                close(fd_out);
            }

            execute_command(args, NULL, NULL);
            exit(EXIT_SUCCESS); // Ensure the child process ends
        }
        else if (pid < 0)
        {
            perror("Error al hacer fork");
            exit(EXIT_FAILURE);
        }
        else
        {
            // Father process
            waitpid(pid, NULL, 0); // Wait for the child process to end
            close(pipe_fds[1]);    // Close the write end of the pipe

            if (prev_fd != -1)
            {
                close(prev_fd); // Close the previous pipe
            }
            prev_fd = pipe_fds[0]; // Update the previous pipe
        }
    }
}

void parse_input(char* input, char** args, char** input_file, char** output_file)
{
    int i = 0;
    *input_file = NULL;
    *output_file = NULL;

    args[i] = strtok(input, " \n");
    while (args[i] != NULL && i < MAX_ARGS - 1)
    {
        if (strcmp(args[i], "<") == 0)
        {
            args[i] = NULL;
            *input_file = strtok(NULL, " \n");
        }
        else if (strcmp(args[i], ">") == 0)
        {
            args[i] = NULL;
            *output_file = strtok(NULL, " \n");
        }
        else
        {
            i++;
            args[i] = strtok(NULL, " \n");
        }
    }
    args[i] = NULL;
}

int parse_pipes(char* input, char** commands)
{
    int count = 0;
    char* token = strtok(input, "|");
    while (token != NULL)
    {
        // Eliminar espacios en blanco al principio y al final del comando
        while (isspace((unsigned char)*token))
            token++;
        char* end = token + strlen(token) - 1;
        while (end > token && isspace((unsigned char)*end))
            end--;
        *(end + 1) = '\0';

        commands[count++] = token;
        token = strtok(NULL, "|");
    }
    commands[count] = NULL; // Termina la lista de comandos
    return count;
}

void execute_batch_file(const char* filename)
{
    // Open the file
    FILE* file = fopen(filename, "r+"); // Open in read/write mode
    if (!file)
    {
        perror("Error while opening the file");
        exit(EXIT_FAILURE);
    }

    char* input_file;
    char* output_file;

    // Read the file line by line
    char line[COMMAND_MAX];
    while (fgets(line, sizeof(line), file))
    { // Same process as in main
        char* args[MAX_ARGS];

        bool background = false;
        if (line[strlen(line) - 2] == '&')
        {
            printf("Background process\n");
            background = true;
            line[strlen(line) - 2] = '\0';
        }

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
            {
                if (strchr(line, '|') != NULL)
                {
                    execute_piped_commands(line);
                }
                else
                {
                    parse_input(line, args, &input_file, &output_file);
                    execute_command(args, input_file, output_file);
                }
                exit(EXIT_SUCCESS);
            }
            else
            {
                job_counter++;
                background_jobs[job_counter - 1].job_id = job_counter;
                background_jobs[job_counter - 1].pid = pid;
                strncpy(background_jobs[job_counter - 1].command, line,
                        sizeof(background_jobs[job_counter - 1].command) - 1);
                background_jobs[job_counter - 1].command[sizeof(background_jobs[job_counter - 1].command) - 1] = '\0';
                printf("[%d] %d\n", job_counter, pid);
                usleep(100);
            }
        }
        else
        {
            if (strchr(line, '|') != NULL)
            {
                execute_piped_commands(line);
            }
            else
            {
                parse_input(line, args, &input_file, &output_file);
                execute_command(args, input_file, output_file);
            }
        }

        if (!background && foreground_pid > 0)
        {
            waitpid(foreground_pid, NULL, 0);
            foreground_pid = -1;
        }

        if (background_job_completed)
        {
            print_completed_jobs();
            background_job_completed = 0;
        }

        // Erase the line by overwriting it with spaces
        fseek(file, -strlen(line), SEEK_CUR);
        for (int i = 0; i < (int)strlen(line); i++)
        {
            fputc(' ', file);
        }
        fseek(file, 0, SEEK_CUR); // Move back to the current position for next read
        // This is neccesary, otherwise it keeps reading the commands over and over
    }

    fclose(file);
}
