#include "expose_metrics.h"

/** Mutex para sincronización de hilos */
pthread_mutex_t lock;

/** Métrica de Prometheus para el uso de CPU */
static prom_gauge_t* cpu_usage_metric;

/** Métrica de Prometheus para el uso de memoria */
static prom_gauge_t* memory_usage_metric;

/** Métrica de Prometheus para la memoria total */
static prom_gauge_t* memory_total_metric;

/** Métrica de Prometheus para la memoria usada */
static prom_gauge_t* memory_used_metric;

/** Métrica de Prometheus para la memoria disponible */
static prom_gauge_t* memory_available_metric;

/** Métrica de Prometheus para cantidad de lecturas completadas */
static prom_gauge_t* disk_reads_completed_metric;

/** Métrica de Prometheus para cantidad de escrituras completadas */
static prom_gauge_t* disk_writes_completed_metric;

/** Métrica de Prometheus para velocidad de lectura */
static prom_gauge_t* disk_read_speed_metric;

/** Métrica de Prometheus para velocidad de escritura */
static prom_gauge_t* disk_write_speed_metric;

/** Métrica de Prometheus para los bytes recibidos */
static prom_gauge_t* net_bytes_recv_metric;

/** Métrica de Prometheus para los paquetes recibidos */
static prom_gauge_t* net_packets_recv_metric;

/** Métrica de Prometheus para los errores de recepción */
static prom_gauge_t* net_errs_recv_metric;

/** Métrica de Prometheus para los bytes transmitidos */
static prom_gauge_t* net_bytes_trans_metric;

/** Métrica de Prometheus para los paquetes transmitidos */
static prom_gauge_t* net_packets_trans_metric;

/** Métrica de Prometheus para los errores de transmisión */
static prom_gauge_t* net_errs_trans_metric;

/** Métrica de Prometheus para los procesos en ejecución */
static prom_gauge_t* processes_running_metric;

/** Métrica de Prometheus para los cambios de contexto */
static prom_gauge_t* context_switches_metric;

void update_cpu_gauge()
{
    double usage = get_cpu_usage();
    if (usage >= 0)
    {
        pthread_mutex_lock(&lock);
        prom_gauge_set(cpu_usage_metric, usage, NULL);
        pthread_mutex_unlock(&lock);
    }
    else
    {
        fprintf(stderr, "Error al obtener el uso de CPU\n");
    }
}

void update_memory_gauge()
{
    MemValues values = get_memory_values();
    if (values.usage >= 0)
    {
        pthread_mutex_lock(&lock);
        prom_gauge_set(memory_usage_metric, values.usage, NULL);
        prom_gauge_set(memory_total_metric, values.total, NULL);
        prom_gauge_set(memory_used_metric, values.used, NULL);
        prom_gauge_set(memory_available_metric, values.available, NULL);
        pthread_mutex_unlock(&lock);
    }
    else
    {
        fprintf(stderr, "Error al obtener el uso de memoria\n");
    }
}

void update_disk_gauge()
{
    DiskStats stats = get_disk_stats("nvme0n1");
    if (stats.reads_completed >= 0)
    {
        pthread_mutex_lock(&lock);
        prom_gauge_set(disk_reads_completed_metric, stats.reads_completed, NULL);
        prom_gauge_set(disk_writes_completed_metric, stats.writes_completed, NULL);
        prom_gauge_set(disk_read_speed_metric, stats.read_speed, NULL);
        prom_gauge_set(disk_write_speed_metric, stats.write_speed, NULL);
        pthread_mutex_unlock(&lock);
    }
    else
    {
        fprintf(stderr, "Error al obtener las estadísticas de disco\n");
    }
}

void update_network_gauge()
{
    NetDevStats stats = get_network_stats("enp5s0");
    if (stats.bytes_recv >= 0)
    {
        pthread_mutex_lock(&lock);
        prom_gauge_set(net_bytes_recv_metric, stats.bytes_recv, NULL);
        prom_gauge_set(net_packets_recv_metric, stats.packets_recv, NULL);
        prom_gauge_set(net_errs_recv_metric, stats.errs_recv, NULL);
        prom_gauge_set(net_bytes_trans_metric, stats.bytes_trans, NULL);
        prom_gauge_set(net_packets_trans_metric, stats.packets_trans, NULL);
        prom_gauge_set(net_errs_trans_metric, stats.errs_trans, NULL);
        pthread_mutex_unlock(&lock);
    }
    else
    {
        fprintf(stderr, "Error al obtener las estadísticas de disco\n");
    }
}

void update_processes_gauge()
{
    int process_count = get_running_processes();
    if (process_count >= 0)
    {
        pthread_mutex_lock(&lock);
        prom_gauge_set(processes_running_metric, process_count, NULL);
        pthread_mutex_unlock(&lock);
    }
    else
    {
        fprintf(stderr, "Error al obtener la cantidad de procesos en ejecución\n");
    }
}

void update_context_switches_gauge()
{
    int context_switches = get_context_switches();
    if (context_switches >= 0)
    {
        pthread_mutex_lock(&lock);
        prom_gauge_set(context_switches_metric, context_switches, NULL);
        pthread_mutex_unlock(&lock);
    }
    else
    {
        fprintf(stderr, "Error al obtener la cantidad de cambios de contexto\n");
    }
}

void* expose_metrics(void* arg)
{
    (void)arg; // Argumento no utilizado

    // Aseguramos que el manejador HTTP esté adjunto al registro por defecto
    promhttp_set_active_collector_registry(NULL);

    // Iniciamos el servidor HTTP en el puerto 8000
    struct MHD_Daemon* daemon = promhttp_start_daemon(MHD_USE_SELECT_INTERNALLY, 8000, NULL, NULL);
    if (daemon == NULL)
    {
        fprintf(stderr, "Error al iniciar el servidor HTTP\n");
        return NULL;
    }

    // Mantenemos el servidor en ejecución
    while (1)
    {
        sleep(1);
    }

    // Nunca debería llegar aquí
    MHD_stop_daemon(daemon);
    return NULL;
}

void init_metrics(bool monitor_cpu_usage, bool monitor_memory_usage, bool monitor_disk, 
                  bool monitor_network, bool monitor_processes_running, bool monitor_context_switches)
{
    // Inicializamos el mutex
    if (pthread_mutex_init(&lock, NULL) != 0)
    {
        fprintf(stderr, "Error al inicializar el mutex\n");
        return;
    }

    // Inicializamos el registro de coleccionistas de Prometheus
    if (prom_collector_registry_default_init() != 0)
    {
        fprintf(stderr, "Error al inicializar el registro de Prometheus\n");
        return;
    }

    // Creamos y registramos la métrica para el uso de CPU
    if (monitor_cpu_usage)
    {
        cpu_usage_metric = prom_gauge_new("cpu_usage_percentage", "Porcentaje de uso de CPU", 0, NULL);
        if (cpu_usage_metric == NULL)
        {
            fprintf(stderr, "Error al crear la métrica de uso de CPU\n");
            return;
        }
        if (!prom_collector_registry_must_register_metric(cpu_usage_metric) != 0)
        {
            fprintf(stderr, "Error al registrar la métrica de la CPU\n");
            return;
        }
    }

    // Creamos y registramos las métricas de memoria
    if (monitor_memory_usage)
    {
        memory_usage_metric = prom_gauge_new("memory_usage_percentage", "Porcentaje de uso de memoria", 0, NULL);
        if (memory_usage_metric == NULL)
        {
            fprintf(stderr, "Error al crear la métrica de uso de memoria\n");
            return;
        }

        memory_total_metric = prom_gauge_new("memory_total", "Memoria total", 0, NULL);
        if (memory_total_metric == NULL)
        {
            fprintf(stderr, "Error al crear la métrica de memoria total\n");
            return;
        }

        memory_used_metric = prom_gauge_new("memory_used", "Memoria usada", 0, NULL);
        if (memory_used_metric == NULL)
        {
            fprintf(stderr, "Error al crear la métrica de memoria usada\n");
            return;
        }

        memory_available_metric = prom_gauge_new("memory_available", "Memoria disponible", 0, NULL);
        if (memory_available_metric == NULL)
        {
            fprintf(stderr, "Error al crear la métrica de memoria disponible\n");
            return;
        }

        if (!prom_collector_registry_must_register_metric(memory_usage_metric) != 0)
        {
            fprintf(stderr, "Error al registrar la métrica de la memoria\n");
            return;
        }

        if (!prom_collector_registry_must_register_metric(memory_total_metric) != 0)
        {
            fprintf(stderr, "Error al registrar la métrica de la memoria total\n");
            return;
        }

        if (!prom_collector_registry_must_register_metric(memory_used_metric) != 0)
        {
            fprintf(stderr, "Error al registrar la métrica de la memoria usada\n");
            return;
        }

        if (!prom_collector_registry_must_register_metric(memory_available_metric) != 0)
        {
            fprintf(stderr, "Error al registrar la métrica de la memoria disponible\n");
            return;
        }
    }

    // Creamos y registramos las métricas para las estadísticas de disco
    if (monitor_disk)
    {
        disk_reads_completed_metric = prom_gauge_new("disk_reads_completed", "Cantidad de lecturas completadas", 0, NULL);
        if (disk_reads_completed_metric == NULL)
        {
            fprintf(stderr, "Error al crear la métrica de lecturas completadas\n");
            return;
        }

        disk_writes_completed_metric =
            prom_gauge_new("disk_writes_completed", "Cantidad de escrituras completadas", 0, NULL);
        if (disk_writes_completed_metric == NULL)
        {
            fprintf(stderr, "Error al crear la métrica de escrituras completadas\n");
            return;
        }

        disk_read_speed_metric = prom_gauge_new("disk_read_speed", "Velocidad de lectura del disco [KB/s]", 0, NULL);
        if (disk_read_speed_metric == NULL)
        {
            fprintf(stderr, "Error al crear la métrica de velocidad de lectura\n");
            return;
        }

        disk_write_speed_metric = prom_gauge_new("disk_write_speed", "Velocidad de escritura del disco [KB/s]", 0, NULL);
        if (disk_write_speed_metric == NULL)
        {
            fprintf(stderr, "Error al crear la métrica de velocidad de escritura\n");
            return;
        }

        if (!prom_collector_registry_must_register_metric(disk_reads_completed_metric) != 0)
        {
            fprintf(stderr, "Error al registrar la métrica de lecturas completadas\n");
            return;
        }

        if (!prom_collector_registry_must_register_metric(disk_writes_completed_metric) != 0)
        {
            fprintf(stderr, "Error al registrar la métrica de escrituras completadas\n");
            return;
        }

        if (!prom_collector_registry_must_register_metric(disk_read_speed_metric) != 0)
        {
            fprintf(stderr, "Error al registrar la métrica de velocidad de lectura\n");
            return;
        }

        if (!prom_collector_registry_must_register_metric(disk_write_speed_metric) != 0)
        {
            fprintf(stderr, "Error al registrar la métrica de velocidad de escritura\n");
            return;
        }
    }

    // Creamos las métricas para las estadísticas de red
    if (monitor_network)
    {
        net_bytes_recv_metric = prom_gauge_new("net_bytes_recv", "Bytes recibidos", 0, NULL);
        if (net_bytes_recv_metric == NULL)
        {
            fprintf(stderr, "Error al crear la métrica de bytes recibidos\n");
            return;
        }

        net_packets_recv_metric = prom_gauge_new("net_packets_recv", "Paquetes recibidos", 0, NULL);
        if (net_packets_recv_metric == NULL)
        {
            fprintf(stderr, "Error al crear la métrica de paquetes recibidos\n");
            return;
        }

        net_errs_recv_metric = prom_gauge_new("net_errs_recv", "Errores de recepción", 0, NULL);
        if (net_errs_recv_metric == NULL)
        {
            fprintf(stderr, "Error al crear la métrica de errores de recepción\n");
            return;
        }

        net_bytes_trans_metric = prom_gauge_new("net_bytes_trans", "Bytes transmitidos", 0, NULL);
        if (net_bytes_trans_metric == NULL)
        {
            fprintf(stderr, "Error al crear la métrica de bytes transmitidos\n");
            return;
        }

        net_packets_trans_metric = prom_gauge_new("net_packets_trans", "Paquetes transmitidos", 0, NULL);
        if (net_packets_trans_metric == NULL)
        {
            fprintf(stderr, "Error al crear la métrica de paquetes transmitidos\n");
            return;
        }

        net_errs_trans_metric = prom_gauge_new("net_errs_trans", "Errores de transmisión", 0, NULL);
        if (net_errs_trans_metric == NULL)
        {
            fprintf(stderr, "Error al crear la métrica de errores de transmisión\n");
            return;
        }

        if (!prom_collector_registry_must_register_metric(net_bytes_recv_metric) != 0)
        {
            fprintf(stderr, "Error al registrar la métrica de bytes recibidos\n");
            return;
        }

        if (!prom_collector_registry_must_register_metric(net_packets_recv_metric) != 0)
        {
            fprintf(stderr, "Error al registrar la métrica de paquetes recibidos\n");
            return;
        }

        if (!prom_collector_registry_must_register_metric(net_errs_recv_metric) != 0)
        {
            fprintf(stderr, "Error al registrar la métrica de errores de recepción\n");
            return;
        }

        if (!prom_collector_registry_must_register_metric(net_bytes_trans_metric) != 0)
        {
            fprintf(stderr, "Error al registrar la métrica de bytes transmitidos\n");
            return;
        }

        if (!prom_collector_registry_must_register_metric(net_packets_trans_metric) != 0)
        {
            fprintf(stderr, "Error al registrar la métrica de paquetes transmitidos\n");
            return;
        }

        if (!prom_collector_registry_must_register_metric(net_errs_trans_metric) != 0)
        {
            fprintf(stderr, "Error al registrar la métrica de errores de transmisión\n");
            return;
        }
    }
    
    // Creamos y registramos la métrica para la cantidad de procesos en ejecución
    if (monitor_processes_running)
    {
        processes_running_metric = prom_gauge_new("processes_running", "Cantidad de procesos en ejecución", 0, NULL);
        if (processes_running_metric == NULL)
        {
            fprintf(stderr, "Error al crear la métrica de procesos en ejecución\n");
            return;
        }

        if (!prom_collector_registry_must_register_metric(processes_running_metric) != 0)
        {
            fprintf(stderr, "Error al registrar la métrica de procesos en ejecución\n");
            return;
        }
    }
    
    // Creamos y registramos la métrica para la cantidad de cambios de contexto
    if (monitor_context_switches)
    {
        context_switches_metric = prom_gauge_new("context_switches", "Cantidad de cambios de contexto", 0, NULL);
        if (context_switches_metric == NULL)
        {
            fprintf(stderr, "Error al crear la métrica de cambios de contexto\n");
            return;
        }

        if (!prom_collector_registry_must_register_metric(context_switches_metric) != 0)
        {
            fprintf(stderr, "Error al registrar la métrica de cambios de contexto\n");
            return;
        }
    }    
}

void destroy_mutex()
{
    pthread_mutex_destroy(&lock);
}
