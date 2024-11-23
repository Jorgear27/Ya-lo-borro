/**
 * @file main.c
 * @brief Entry point of the system
 */

#include "expose_metrics.h"
#include <fcntl.h> 
#include <cjson/cJSON.h>

/**
 * @brief Ruta del pipe para recibir configuraciones.
 */
#define PIPE_PATH "/tmp/monitor_pipe"

/**
 * @brief Configuración global.
 */
int sleep_time = 1;
bool monitor_cpu_usage = true;
bool monitor_memory_usage = true;
bool monitor_disk = true;
bool monitor_network = true;
bool monitor_processes_running = true;
bool monitor_context_switches = true;

/**
 * @brief Punto de entrada del programa.
 *
 * @param argc Cantidad de argumentos.
 * @param argv Argumentos del programa.
 * @return int Código de salida.
 */
int main(int argc, char* argv[])
{   
    // Avoid unused variable warning
    (void)argc;
    (void)argv;
    
    int pipe_fd = open(PIPE_PATH, O_RDONLY | O_NONBLOCK);
    if (pipe_fd == -1) {
        perror("Failed to open pipe");
        return EXIT_FAILURE;
    }

    usleep(1000000); // 1 s, wait for the Shell to write the configuration

    char buffer[BUFFER_SIZE];
    ssize_t bytes_read;
    while ((bytes_read = read(pipe_fd, buffer, sizeof(buffer) - 1)) == -1 && errno == EAGAIN) {
        // Wait until data is available
        usleep(100000); // 100 ms
    }

    if (bytes_read == -1) {
        perror("Failed to read from pipe");
        close(pipe_fd);
        return EXIT_FAILURE;
    }
    buffer[bytes_read] = '\0';
    close(pipe_fd);

    cJSON *json = cJSON_Parse(buffer);
    if (!json) {
        fprintf(stderr, "Error parsing config JSON: Default configuration will be used%s\n", cJSON_GetErrorPtr());
    } else {
        cJSON *sleep_time_item = cJSON_GetObjectItem(json, "sleep_time");
        if (cJSON_IsNumber(sleep_time_item)) {
            sleep_time = sleep_time_item->valueint;
        }

        cJSON *metrics = cJSON_GetObjectItem(json, "metrics");
        if (metrics) {
            monitor_cpu_usage = cJSON_IsTrue(cJSON_GetObjectItem(metrics, "cpu_usage"));
            monitor_memory_usage = cJSON_IsTrue(cJSON_GetObjectItem(metrics, "memory_usage"));
            monitor_disk = cJSON_IsTrue(cJSON_GetObjectItem(metrics, "disk_usage"));
            monitor_network = cJSON_IsTrue(cJSON_GetObjectItem(metrics, "network_usage"));
            monitor_processes_running = cJSON_IsTrue(cJSON_GetObjectItem(metrics, "processes_running"));
            monitor_context_switches = cJSON_IsTrue(cJSON_GetObjectItem(metrics, "context_switches"));
        }
    }

    cJSON_Delete(json);

    // Create a thread to expose metrics via HTTP
    pthread_t metrics_tid;
    if (pthread_create(&metrics_tid, NULL, expose_metrics, NULL) != 0) {
        fprintf(stderr, "Error creating HTTP server thread\n");
        return EXIT_FAILURE;
    }

    // Initialize mutex and metrics
    init_metrics(monitor_cpu_usage, monitor_memory_usage, monitor_disk, 
                 monitor_network, monitor_processes_running, monitor_context_switches);  

    // Main loop to update metrics 
    while (true) {
        if (monitor_cpu_usage) update_cpu_gauge();
        if (monitor_memory_usage) update_memory_gauge();
        if (monitor_disk) update_disk_gauge();
        if (monitor_network) update_network_gauge();
        if (monitor_processes_running) update_processes_gauge();
        if (monitor_context_switches) update_context_switches_gauge();
        sleep(sleep_time);
    }

    return EXIT_SUCCESS;
}
