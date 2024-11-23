/**
 * @file utils.h
 * @brief Contains the declarations of the functions that are used in the main program.
 *
 */

#include <limits.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

/**
 * @brief Maximum number of characters in a command
 *
 */
#define COMMAND_MAX 1024

/**
 * @brief Maximum number of arguments supported
 *
 */
#define MAX_ARGS 64

/**
 * @brief Maximum number of jobs in the background
 *
 */
#define MAX_JOBS 10

/**
 * @brief Structure to store the background jobs
 *
 */
typedef struct
{
    /**
     * @brief Job ID
     *
     */
    int job_id;
    /**
     * @brief Process ID
     *
     */
    pid_t pid;
    /**
     * @brief Command
     *
     */
    char command[COMMAND_MAX];
} BackgroundJob;

/**
 * @brief Variable to store the PID of the process in the foreground
 *
 */
extern pid_t foreground_pid;

/**
 * @brief Counter for the number of jobs in the background
 *
 */
extern int job_counter;

/**
 * @brief Variables to store the most recent job
 *
 */
extern int most_recent_job;

/**
 * @brief Variables to store the second most recent job
 *
 */
extern int second_most_recent_job;

/**
 * @brief Array to store the background jobs
 *
 */
extern BackgroundJob background_jobs[MAX_JOBS];

/**
 * @brief Variable to store the completion status of a background job
 *
 */
extern int background_job_completed;

/**
 * @brief Displays the prompt in the terminal.
 *
 */
void display_prompt(void);

/**
 * @brief Signal handler for SIGINT and SIGTSTP
 *
 * @param sig Signal number
 */
void signal_handler(int sig);

/**
 * @brief Prints the completed background jobs.
 *
 */
void print_completed_jobs(void);

/**
 * @brief Prints the welcome message when the shell is started.
 *
 */
void print_welcome_message(void);
