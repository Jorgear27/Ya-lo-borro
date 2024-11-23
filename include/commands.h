/**
 * @file commands.h
 * @author Jorge Arbach (jorge.arbach@mi.unc.edu.ar)
 * @brief Functions to handle shell commands
 * @version 0.1
 * @date 2024-11-13
 *
 * @copyright Copyright (c) 2024
 *
 */

#include "monitor_commands.h"
#include <ctype.h>
#include <fcntl.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

/**
 * @brief Internal commands supported by the shell
 *
 */
extern char* internal_commands[];

/**
 * @brief 'cd' command to change the current directory
 * @param path The path to change to
 *
 */
void change_directory(const char* path);

/**
 * @brief 'clr' command to clear the screen
 *
 */
void clear_screen(void);

/**
 * @brief 'echo' command to print a message
 * @param message The message to print
 *
 */
void echo(const char* message);

/**
 * @brief 'quit' command to exit the shell
 *
 */
void quit_shell(void);

/**
 * @brief 'help' command to display the help message
 *
 */
void show_help(void);

/**
 * @brief Checks if a command is internal
 *
 * @param args command
 * @return true if the command is internal
 * @return false if the command is external
 */
bool is_internal_command(char** args);

/**
 * @brief Executes internal commands of the shell
 *
 * @param args external command
 * @param input_file input file for redirection
 * @param output_file output file for redirection
 */
void execute_command(char** args, char* input_file, char* output_file);

/**
 * @brief Executes external programs.
 *
 * @param args command and arguments
 */
void execute_program(char** args);

/**
 * @brief Executes piped commands.
 *
 * @param input User input.
 */
void execute_piped_commands(char* input);

/**
 * @brief Parse the input command
 *
 * @param input The input command string
 * @param args The arguments array to populate
 * @param input_file The input file for redirection
 * @param output_file The output file for redirection
 *
 */
void parse_input(char* input, char** args, char** input_file, char** output_file);

/**
 * @brief Parse pipes in the input command
 *
 * @param input The input command string
 * @param commands The commands array to populate
 * @return int The number of commands
 */
int parse_pipes(char* input, char** commands);

/**
 * @brief Executes the commands in a batch file.
 *
 */
void execute_batch_file(const char* filename);
