/**
 * @file metrics.h
 * @brief Funciones para obtener el uso de CPU y memoria desde el sistema de archivos /proc.
 */

#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

/**
 * @brief Tamaño del buffer para leer archivos.
 */
#define BUFFER_SIZE 1024

/**
 * @brief Estructura para almacenar los valores de memoria.
 */
typedef struct
{
    /**
     * @brief Memoria total en KB.
     *
     */
    double total;
    /**
     * @brief Memoria usada en KB.
     *
     */
    double used;
    /**
     * @brief Memoria disponible en KB.
     *
     */
    double available;
    /**
     * @brief Porcentaje de uso de memoria.
     *
     */
    double usage;
} MemValues;

/**
 * @brief Estructura para almacenar las estadísticas de disco.
 */
typedef struct
{
    /**
     * @brief Cantidad de lecturas completadas.
     *
     */
    double reads_completed;
    /**
     * @brief Cantidad de escrituras completadas.
     *
     */
    double writes_completed;
    /**
     * @brief Velocidad de lectura en KB/s.
     *
     */
    double read_speed;
    /**
     * @brief Velocidad de escritura en KB/s.
     *
     */
    double write_speed;
} DiskStats;

/**
 * @brief Estructura para almacenar las estadísticas de red.
 */
typedef struct
{
    /**
     * @brief Cantidad de bytes recibidos.
     *
     */
    double bytes_recv;
    /**
     * @brief Cantidad de paquetes recibidos.
     *
     */
    double packets_recv;
    /**
     * @brief Cantidad de errores de recepción.
     *
     */
    double errs_recv;
    /**
     * @brief Cantidad de bytes transmitidos.
     *
     */
    double bytes_trans;
    /**
     * @brief Cantidad de paquetes transmitidos.
     *
     */
    double packets_trans;
    /**
     * @brief Cantidad de errores de transmisión.
     *
     */
    double errs_trans;
} NetDevStats;

/**
 * @brief Obtiene los valores de memoria total, usada y disponible desde /proc/meminfo.
 *
 * Lee los valores de memoria total, usada y disponible desde /proc/meminfo y los
 * almacena en una estructura MemValues.
 *
 * @return Estructura MemValues con los valores de memoria, o valores en -1.0 en caso de error.
 */
MemValues get_memory_values();

/**
 * @brief Obtiene el porcentaje de uso de CPU desde /proc/stat.
 *
 * Lee los tiempos de CPU desde /proc/stat y calcula el porcentaje de uso de CPU
 * en un intervalo de tiempo.
 *
 * @return Uso de CPU como porcentaje (0.0 a 100.0), o -1.0 en caso de error.
 */
double get_cpu_usage();

/**
 * @brief Obtiene las estadísticas de disco desde /proc/diskstats.
 *
 * Lee las estadísticas de disco desde /proc/diskstats y calcula la cantidad de bytes
 * leídos y escritos en un intervalo de tiempo.
 *
 * @return Estructura DiskStats con las estadísticas de disco, o valores en -1.0 en caso de error.
 */
DiskStats get_disk_stats(const char* device_name_filter);

/**
 * @brief Obtiene las estadísticas de red desde /proc/net/dev.
 *
 * Lee las estadísticas de red desde /proc/net/dev y calcula la cantidad de bytes y paquetes
 * recibidos y transmitidos en un intervalo de tiempo.
 *
 * @return Estructura NetDevStats con las estadísticas de red, o valores en -1 en caso de error.
 */
NetDevStats get_network_stats(const char* interface_name_filter);

/**
 * @brief Calcula la cantidad de procesos en ejecución desde /proc
 *
 * Lee la cantidad de directorios en /proc y calcula la cantidad que corresponden
 * a procesos en ejecución.
 *
 * @return Cantidad de procesos.
 */
int get_running_processes();

/**
 * @brief Obtiene la cantidad de cambios de contexto desde /proc/stat
 *
 * Lee la cantidad de cambios de contexto desde /proc/stat y la retorna.
 *
 * @return Cantidad de cambios de contexto.
 */
unsigned long long get_context_switches();
