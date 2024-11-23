#include "metrics.h"

#define INTERFACE_NAME_MAX 128

MemValues get_memory_values()
{
    FILE* fp;
    char buffer[BUFFER_SIZE];
    unsigned long long total_mem = 0, free_mem = 0;

    // Abrir el archivo /proc/meminfo
    fp = fopen("/proc/meminfo", "r");
    if (fp == NULL)
    {
        perror("Error al abrir /proc/meminfo");
        MemValues values = {-1.0, -1.0, -1.0, -1.0};
        return values;
    }

    // Leer los valores de memoria total y disponible
    while (fgets(buffer, sizeof(buffer), fp) != NULL)
    {
        if (sscanf(buffer, "MemTotal: %llu kB", &total_mem) == 1)
        {
            continue; // MemTotal encontrado
        }
        if (sscanf(buffer, "MemAvailable: %llu kB", &free_mem) == 1)
        {
            break; // MemAvailable encontrado, podemos dejar de leer
        }
    }

    fclose(fp);

    // Verificar si se encontraron ambos valores
    if (total_mem == 0 || free_mem == 0)
    {
        fprintf(stderr, "Error al leer la información de memoria desde /proc/meminfo\n");
        MemValues values = {-1.0, -1.0, -1.0, -1.0};
        return values;
    }

    // Calcular el porcentaje de uso de memoria
    double used_mem = total_mem - free_mem;
    double mem_usage_percent = (used_mem / total_mem) * 100.0;

    MemValues values = {total_mem, used_mem, free_mem, mem_usage_percent};
    return values;
}

double get_cpu_usage()
{
    static unsigned long long prev_user = 0, prev_nice = 0, prev_system = 0, prev_idle = 0, prev_iowait = 0,
                              prev_irq = 0, prev_softirq = 0, prev_steal = 0;
    unsigned long long user, nice, system, idle, iowait, irq, softirq, steal;
    unsigned long long totald, idled;
    double cpu_usage_percent;

    // Abrir el archivo /proc/stat
    FILE* fp = fopen("/proc/stat", "r");
    if (fp == NULL)
    {
        perror("Error al abrir /proc/stat");
        return -1.0;
    }

    char buffer[BUFFER_SIZE * 4];
    if (fgets(buffer, sizeof(buffer), fp) == NULL)
    {
        perror("Error al leer /proc/stat");
        fclose(fp);
        return -1.0;
    }
    fclose(fp);

    // Analizar los valores de tiempo de CPU
    int ret = sscanf(buffer, "cpu  %llu %llu %llu %llu %llu %llu %llu %llu", &user, &nice, &system, &idle, &iowait,
                     &irq, &softirq, &steal);
    if (ret < 8)
    {
        fprintf(stderr, "Error al parsear /proc/stat\n");
        return -1.0;
    }

    // Calcular las diferencias entre las lecturas actuales y anteriores
    unsigned long long prev_idle_total = prev_idle + prev_iowait;
    unsigned long long idle_total = idle + iowait;

    unsigned long long prev_non_idle = prev_user + prev_nice + prev_system + prev_irq + prev_softirq + prev_steal;
    unsigned long long non_idle = user + nice + system + irq + softirq + steal;

    unsigned long long prev_total = prev_idle_total + prev_non_idle;
    unsigned long long total = idle_total + non_idle;

    totald = total - prev_total;
    idled = idle_total - prev_idle_total;

    if (totald == 0)
    {
        fprintf(stderr, "Total es cero, no se puede calcular el uso de CPU!\n");
        return -1.0;
    }

    // Calcular el porcentaje de uso de CPU
    cpu_usage_percent = ((double)(totald - idled) / totald) * 100.0;

    // Actualizar los valores anteriores para la siguiente lectura
    prev_user = user;
    prev_nice = nice;
    prev_system = system;
    prev_idle = idle;
    prev_iowait = iowait;
    prev_irq = irq;
    prev_softirq = softirq;
    prev_steal = steal;

    return cpu_usage_percent;
}

DiskStats get_disk_stats(const char* device_name_filter)
{
    FILE* fp;
    char buffer[BUFFER_SIZE];
    unsigned long long reads_completed = 0, sectors_read = 0, time_reading = 0;
    unsigned long long writes_completed = 0, sectors_written = 0, time_writing = 0;
    const int sector_size = 512; // Tamaño del sector de nvme01 en bytes (lo averiguamos mediante el comando lsblk -b -o
                                 // NAME,PHY-SEC,LOG-SEC).

    // Abrir el archivo /proc/diskstats
    fp = fopen("/proc/diskstats", "r");
    if (fp == NULL)
    {
        perror("Error al abrir /proc/diskstats");
        DiskStats stats = {-1.0, -1.0, -1.0, -1.0};
        return stats;
    }

    // Leer el archivo línea por línea
    while (fgets(buffer, sizeof(buffer), fp) != NULL)
    {
        char device_name[32];

        // Parsear los valores importantes de la línea
        sscanf(buffer, "%*u %*u %s %llu %*u %llu %llu %llu %*u %llu %llu", device_name, &reads_completed, &sectors_read,
               &time_reading, &writes_completed, &sectors_written, &time_writing);

        // Filtrar sólo el dispositivo deseado (por ejemplo, sda o nvme0n1)
        if (strncmp(device_name, device_name_filter, strlen(device_name_filter)) == 0)
        {
            break; // Encontrado el dispositivo deseado, podemos detener la lectura.
        }
    }

    fclose(fp);

    // Calcular los bytes leídos y escritos a partir de los sectores
    double bytes_read = sectors_read * sector_size;
    double bytes_written = sectors_written * sector_size;

    // Calcular la velocidad de lectura y escritura en KiloBytes por segundo
    double read_speed = bytes_read / time_reading;
    double write_speed = bytes_written / time_writing;

    DiskStats stats = {reads_completed, writes_completed, read_speed, write_speed};
    return stats;
}

NetDevStats get_network_stats(const char* interface_name_filter)
{
    FILE* fp = fopen("/proc/net/dev", "r");
    char buffer[BUFFER_SIZE];
    if (fp == NULL)
    {
        perror("Error al abrir /proc/net/dev");
        NetDevStats stats = {-1.0, -1.0, -1.0, -1.0, -1.0, -1.0};
        return stats;
    }

    unsigned long long bytes_recv = 0, packets_recv = 0, errs_recv = 0;
    unsigned long long bytes_trans = 0, packets_trans = 0, errs_trans = 0;

    // Leer el archivo línea por línea
    while (fgets(buffer, sizeof(buffer), fp) != NULL)
    {
        char interface[INTERFACE_NAME_MAX];

        // Parsear los valores importantes de la línea
        sscanf(buffer, "%s %llu %llu %llu %*u  %*u %*u %*u %*u %llu %llu %llu", interface, &bytes_recv, &packets_recv,
               &errs_recv, &bytes_trans, &packets_trans, &errs_trans);

        // Filtrar solo la interfaz deseada
        if (strncmp(interface, interface_name_filter, strlen(interface_name_filter)) == 0)
        {
            break; // Encontrado la interfaz deseada
        }
    }

    fclose(fp);

    NetDevStats stats = {
        .bytes_recv = bytes_recv,
        .packets_recv = packets_recv,
        .errs_recv = errs_recv,
        .bytes_trans = bytes_trans,
        .packets_trans = packets_trans,
        .errs_trans = errs_trans,
    };
    return stats;
}

int get_running_processes()
{
    int process_count = 0;
    DIR* proc_dir = opendir("/proc");

    if (proc_dir == NULL)
    {
        perror("Error al abrir /proc");
        exit(EXIT_FAILURE);
    }

    struct dirent* entry;
    while ((entry = readdir(proc_dir)) != NULL)
    {
        // Verificar si el nombre es numérico, lo que indica que es un proceso
        if (atoi(entry->d_name) != 0)
        {
            process_count++;
        }
    }

    closedir(proc_dir);
    return process_count;
}

unsigned long long get_context_switches()
{
    FILE* fp = fopen("/proc/stat", "r");
    char buffer[BUFFER_SIZE];
    unsigned long long context_switches = 0;

    if (fp == NULL)
    {
        perror("Error al abrir /proc/stat");
        exit(EXIT_FAILURE);
    }

    // Leer el archivo línea por línea
    while (fgets(buffer, sizeof(buffer), fp) != NULL)
    {
        // Buscar la línea que comienza con "ctxt"
        if (strncmp(buffer, "ctxt", 4) == 0)
        {
            sscanf(buffer, "ctxt %llu", &context_switches);
            break;
        }
    }

    fclose(fp);

    if (context_switches == 0)
    {
        fprintf(stderr, "Error al leer la información de los cambios de contexto desde /proc/stat\n");
        return (unsigned long long)-1;
    }

    return context_switches;
}
