#include "monitor_commands.h"

pid_t monitor_pid = -1;

// Monitor commands supported by the shell
char* monitor_commands[] = {"start_monitor", "stop_monitor", "status_monitor", "update_config", "show_config", NULL};

void stop_monitor(void)
{
    if (monitor_pid != -1)
    {
        kill(monitor_pid, SIGTERM);
        waitpid(monitor_pid, NULL, 0);
        monitor_pid = -1;
        printf("Monitor stopped.\n");
    }
    else
    {
        printf("Monitor is no longer running.\n");
    }
}

void status_monitor(void)
{
    if (monitor_pid != -1)
    {
        printf("Monitor is running with PID %d.\n", monitor_pid);
    }
    else
    {
        printf("Monitor is not running.\n");
    }
}

void update_configuration(const char* key, const char* value, const char* config_filename)
{
    // Check if the config file exists
    if (access(config_filename, F_OK) == -1)
    {
        perror("Config file not found");
        printf("Ensure that you are in the correct directory: root_of_the_project/build\n");
        return;
    }

    FILE* file = fopen(config_filename, "r+");
    if (!file)
    {
        perror("Failed to open config file");
        return;
    }

    fseek(file, 0, SEEK_END);
    long length = ftell(file);
    fseek(file, 0, SEEK_SET);
    char* data = malloc(length + 1);
    if (!data)
    {
        perror("Failed to allocate memory");
        fclose(file);
        return;
    }

    fread(data, 1, length, file);
    data[length] = '\0';
    fclose(file);

    cJSON* json = cJSON_Parse(data);
    free(data);
    if (!json)
    {
        fprintf(stderr, "Error parsing config file\n");
        return;
    }

    cJSON* metrics = cJSON_GetObjectItem(json, "metrics");
    cJSON* item = NULL;

    if (strcmp(key, "sleep_time") == 0)
    {
        item = cJSON_GetObjectItem(json, key);
    }
    else if (metrics)
    {
        item = cJSON_GetObjectItem(metrics, key);
    }

    if (item)
    {
        cJSON* new_item = NULL;
        if (strcmp(key, "sleep_time") == 0)
        {
            int interval = atoi(value);
            if (interval < 1 || interval > 10000)
            {
                fprintf(stderr, "Invalid value for sleep_time: %s\n", value);
                cJSON_Delete(json);
                return;
            }
            new_item = cJSON_CreateNumber(interval);
        }
        else
        {
            if (strcmp(value, "true") == 0 || strcmp(value, "1") == 0)
            {
                new_item = cJSON_CreateBool(1);
            }
            else if (strcmp(value, "false") == 0 || strcmp(value, "0") == 0)
            {
                new_item = cJSON_CreateBool(0);
            }
            else
            {
                fprintf(stderr, "Invalid value for key: %s. Only 'true(1)' or 'false(0)' are allowed.\n", key);
                cJSON_Delete(json);
                return;
            }
        }
        if (new_item)
        {
            if (strcmp(key, "sleep_time") == 0)
            {
                cJSON_ReplaceItemInObject(json, key, new_item);
            }
            else
            {
                cJSON_ReplaceItemInObject(metrics, key, new_item);
            }
        }
    }
    else
    {
        fprintf(stderr, "Invalid key: %s\n", key);
        cJSON_Delete(json);
        return;
    }

    char* updated_data = cJSON_Print(json);
    cJSON_Delete(json);
    if (!updated_data)
    {
        fprintf(stderr, "Error printing updated JSON\n");
        return;
    }

    file = fopen(config_filename, "w");
    if (!file)
    {
        perror("Failed to open config file for writing");
        free(updated_data);
        return;
    }

    fwrite(updated_data, 1, strlen(updated_data), file);
    fclose(file);
    free(updated_data);
    printf("Config file updated successfully\n");
}

bool is_monitor_command(char** args)
{
    for (int i = 0; monitor_commands[i] != NULL; i++)
    {
        if (strcmp(args[0], monitor_commands[i]) == 0)
        {
            return true;
        }
    }
    return false;
}

void show_configuration(const char* config_filename)
{
    // Check if the config file exists
    if (access(config_filename, F_OK) == -1)
    {
        perror("Config file not found");
        printf("Ensure that you are in the correct directory: root_of_the_project/build\n");
        return;
    }

    FILE* file = fopen(config_filename, "r");
    if (!file)
    {
        perror("Failed to open config file");
        return;
    }

    fseek(file, 0, SEEK_END);
    long length = ftell(file);
    fseek(file, 0, SEEK_SET);
    char* data = malloc(length + 1);
    if (!data)
    {
        perror("Failed to allocate memory");
        fclose(file);
        return;
    }

    fread(data, 1, length, file);
    data[length] = '\0';
    fclose(file);

    cJSON* json = cJSON_Parse(data);
    free(data);
    if (!json)
    {
        fprintf(stderr, "Error parsing config file\n");
        return;
    }

    char* config_string = cJSON_Print(json);
    cJSON_Delete(json);
    if (!config_string)
    {
        fprintf(stderr, "Error printing JSON\n");
        return;
    }

    printf("Current configuration:\n%s\n", config_string);
    free(config_string);
}

void start_monitor(const char* config_filename)
{
    // Check if the executable exists
    if (access(MONITOR_PATH, X_OK) == -1)
    {
        perror("Monitor executable not found");
        printf("Ensure that you are in the correct directory: root_of_the_project/build\n");
        return;
    }

    // Create the named pipe
    mkfifo(PIPE_PATH, 0666);

    if (monitor_pid != -1)
    {
        printf("Monitor is already running with PID %d.\n", monitor_pid);
        return;
    }

    monitor_pid = fork();
    if (monitor_pid == -1)
    {
        perror("Failed to fork");
        return;
    }

    if (monitor_pid == 0)
    {
        // Child process
        char* args[] = {MONITOR_PATH, NULL};
        execve(args[0], args, environ);
        perror("Failed to execve");
        exit(EXIT_FAILURE);
    }
    else
    {
        // Parent process
        usleep(10000); // 10 ms, wait for the monitor to start
        int pipe_fd = open(PIPE_PATH, O_WRONLY);
        if (pipe_fd == -1)
        {
            perror("Failed to open pipe");
            kill(monitor_pid, SIGTERM);
            waitpid(monitor_pid, NULL, 0);
            monitor_pid = -1;
            return;
        }

        FILE* file = fopen(config_filename, "r");
        if (!file)
        {
            perror("Failed to open config file");
            kill(monitor_pid, SIGTERM);
            waitpid(monitor_pid, NULL, 0);
            monitor_pid = -1;
            close(pipe_fd);
            return;
        }

        fseek(file, 0, SEEK_END);
        long length = ftell(file);
        fseek(file, 0, SEEK_SET);
        char* data = malloc(length + 1);
        if (!data)
        {
            perror("Failed to allocate memory");
            fclose(file);
            kill(monitor_pid, SIGTERM);
            waitpid(monitor_pid, NULL, 0);
            monitor_pid = -1;
            close(pipe_fd);
            return;
        }

        fread(data, 1, length, file);
        data[length] = '\0';
        fclose(file);

        if (write(pipe_fd, data, length) == -1)
        {
            perror("Failed to write to pipe");
        }
        free(data);
        printf("Monitor started with PID %d.\n", monitor_pid);
    }
}
