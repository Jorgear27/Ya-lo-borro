/**
 * @file monitor_commands.h
 * @author Jorge Arbach (jorge.arbach@mi.unc.edu.ar)
 * @brief Functions to handle monitoring commands
 * @version 0.1
 * @date 2024-11-13
 *
 * @copyright Copyright (c) 2024
 *
 */

#include <cjson/cJSON.h>
#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

/**
 * @brief Path of the configuration file.
 *
 */
#define CONFIG_FILE "../config.json"

/**
 * @brief Path of the pipe to communicate with the monitoring program.
 *
 */
#define PIPE_PATH "/tmp/monitor_pipe"

/**
 * @brief Path of the monitoring program.
 *
 */
#define MONITOR_PATH "../Metrics_Monitoring_System/build/metrics"

/**
 * @brief Executable of the monitoring program.
 *
 */
#define MONITOR_EXECUTABLE "./metrics"

/**
 * @brief Monitor commands supported by the shell
 *
 */
extern char* monitor_commands[];

/**
 * @brief PID of the monitoring program.
 *
 */
extern pid_t monitor_pid;

/**
 * @brief Holds the environment variables for the current process
 *
 */
extern char** environ;

/**
 * @brief Start the monitoring of the system.
 *
 * @param config_filename Path of the configuration file.
 */
void start_monitor(const char* config_filename);

/**
 * @brief Stop the monitoring of the system.
 *
 */
void stop_monitor(void);

/**
 * @brief Display the status of the monitoring.
 *
 */
void status_monitor(void);

/**
 * @brief Check if the command is a monitoring command.
 *
 * @param args Command to check.
 * @return true if the command is a monitoring command.
 * @return false if the command is not a monitoring command.
 */
bool is_monitor_command(char** args);

/**
 * @brief Update the configuration of the monitoring program.
 *
 * @param key Key of the configuration to update.
 * @param value Value of the configuration to update.
 * @param config_filename Path of the configuration file.
 */
void update_configuration(const char* key, const char* value, const char* config_filename);

/**
 * @brief Show the current configuration of the monitoring program.
 *
 * @param config_filename Path of the configuration file.
 */
void show_configuration(const char* config_filename);
