#include "utils.h"

// Initialize the global variables
pid_t foreground_pid = -1;
int job_counter = 0;
BackgroundJob background_jobs[MAX_JOBS];
int background_job_completed = 0;
int most_recent_job = -1;
int second_most_recent_job = -1;

void display_prompt(void)
{
    char hostname[HOST_NAME_MAX];
    char cwd[PATH_MAX];
    char* username = getenv("USER");

    if (gethostname(hostname, sizeof(hostname)) == -1)
    {
        perror("Error al obtener el nombre del host");
        return;
    }

    if (getcwd(cwd, sizeof(cwd)) == NULL)
    {
        perror("Error al obtener el directorio actual");
        return;
    }

    // ANSI escape codes for colors
    const char* color_username = "\033[1;32m"; // Bold Green
    const char* color_hostname = "\033[1;34m"; // Bold Blue
    const char* color_cwd = "\033[1;33m";      // Bold Yellow
    const char* color_reset = "\033[0m";       // Reset color

    printf("%s%s%s@%s%s%s:%s%s%s$ ", color_username, username, color_reset, color_hostname, hostname, color_reset,
           color_cwd, cwd, color_reset);
    fflush(stdout); // Ensure the prompt is displayed
}

void signal_handler(int sig)
{
    if (sig == SIGCHLD)
    {
        int status;
        pid_t pid;
        while ((pid = waitpid(-1, &status, WNOHANG)) > 0)
        {
            for (int i = 0; i < job_counter; i++)
            {
                if (background_jobs[i].pid == pid)
                {
                    background_jobs[i].pid = -1;  // Mark the job as completed
                    background_job_completed = 1; // Set the flag
                    second_most_recent_job = most_recent_job;
                    most_recent_job = background_jobs[i].job_id;
                    break;
                }
            }
        }
    }
    else if (sig == SIGINT)
    {
        if (foreground_pid > 0)
        {
            // Send the signal to the process in the foreground
            kill(foreground_pid, sig);
        }
    }
}

void print_completed_jobs()
{
    for (int i = 0; i < job_counter; i++)
    {
        if (background_jobs[i].pid == -1)
        {
            char sign = ' ';
            if (background_jobs[i].job_id == most_recent_job)
            {
                sign = '+';
            }
            else if (background_jobs[i].job_id == second_most_recent_job)
            {
                sign = '-';
            }
            printf("[%d]%c  Done                    %s\n", background_jobs[i].job_id, sign, background_jobs[i].command);
            // Remove the job from the list
            for (int j = i; j < job_counter - 1; j++)
            {
                background_jobs[j] = background_jobs[j + 1];
            }
            job_counter--;
            i--; // Adjust index after removal
        }
    }
}

void print_welcome_message()
{
    // Get current date and time
    time_t now = time(NULL);
    struct tm* t = localtime(&now);
    char date_time[100];
    strftime(date_time, sizeof(date_time) - 1, "%Y-%m-%d %H:%M:%S", t);

    printf("*********************************************************\n");
    printf("*        _          _____  _    _  ______ _      _       *\n");
    printf("*       | |        / ____|| |  | ||  ____| |    | |      *\n");
    printf("*       | |  ___  | (___  | |__| || |__  | |    | |      *\n");
    printf("*  _    | | |___|  \\___ \\ |  __  ||  __| | |    | |      *\n");
    printf("* | \\___| |        ____) || |  | || |____| |____| |____  *\n");
    printf("*  \\_____/        |_____/ |_|  |_||______|______|______| *\n");
    printf("*                                                        *\n");
    printf("*        Welcome to my shell!                            *\n");
    printf("*        Developed by Jorge Arbach.                      *\n");
    printf("*        Write 'help' to see available commmands         *\n");
    printf("*                                                        *\n");
    printf("* Date and time: %s                     *\n", date_time);
    printf("********************************************************\n");
}
