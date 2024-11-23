/**
 * @file expose_metrics.h
 * @brief Programa para leer el uso de CPU y memoria y exponerlos como métricas de Prometheus.
 */

#include "metrics.h"
#include <stdbool.h>
#include <errno.h>
#include <prom.h>
#include <promhttp.h>
#include <pthread.h>

/**
 * @brief Actualiza la métrica de uso de CPU.
 */
void update_cpu_gauge();

/**
 * @brief Actualiza las métricas de uso de memoria.
 */
void update_memory_gauge();

/**
 * @brief Actualiza la métrica de uso de disco.
 */
void update_disk_gauge();

/**
 * @brief Actualiza las métricas de red.
 */
void update_network_gauge();

/**
 * @brief Actualiza la métrica de procesos en ejecución.
 */
void update_processes_gauge();

/**
 * @brief Actualiza la métrica de cambios de contexto.
 */
void update_context_switches_gauge();

/**
 * @brief Función del hilo para exponer las métricas vía HTTP en el puerto 8000.
 * @param arg Argumento no utilizado.
 * @return NULL
 */
void* expose_metrics(void* arg);

/**
 * @brief Inicializa las métricas y el mutex.
 * 
 * @param monitor_cpu_usage true si se debe monitorear el uso de CPU, false en caso contrario.
 * @param monitor_memory_usage true si se debe monitorear el uso de memoria, false en caso contrario.
 * @param monitor_disk true si se debe monitorear las estadisticas de disco, false en caso contrario.
 * @param monitor_network true si se debe monitorear las estadisticas de red, false en caso contrario.
 * @param monitor_processes_running true si se debe monitorear la cantidad de procesos en ejecución, false en caso contrario.
 * @param monitor_context_switches true si se debe monitorear la cantidad de cambios de contexto, false en caso contrario.
 */
void init_metrics(bool monitor_cpu_usage, bool monitor_memory_usage, bool monitor_disk, 
                  bool monitor_network, bool monitor_processes_running, bool monitor_context_switches);

/**
 * @brief Destructor de mutex
 */
void destroy_mutex();
