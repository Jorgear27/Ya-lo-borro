# Instalación y Uso de la Shell y el Programa de Monitoreo

## Requisitos Previos

Asegúrate de tener instaladas las siguientes dependencias en tu sistema:

- **GNU Make**: para ejecutar las tareas de compilación.
- **CMake**: para configurar el proceso de compilación.
- **gcc** o **clang**: el compilador C.
- **libmicrohttpd-dev**: biblioteca para manejar servidores HTTP.
- **Python 3**: para gestionar el entorno virtual y Conan.
- **pip**: para instalar paquetes de Python.

En sistemas basados en Debian/Ubuntu, puedes instalar estas dependencias ejecutando:

```bash
sudo apt update
sudo apt install make cmake gcc libmicrohttpd-dev python3 python3-pip
```

## Clonar el Repositorio

Clona el repositorio del proyecto en tu máquina local:

```bash
gh repo clone ICOMP-UNC/so-i-24-chp2-Jorgear27
cd so-i-24-chp2-Jorgear27
```

## Compilar el Pryecto

### Paso 1: Configurar el Entorno Virtual

Configura un entorno virtual de Python para gestionar las dependencias:

```bash
python3 -m venv venv
source venv/bin/activate
pip install conan
```

### Paso 2: Instalar Dependencias con Conan

Instala las dependencias del proyecto utilizando Conan:

```bash
conan profile detect --force
conan install . --build=missing --settings=build_type=Debug
```

### Paso 3: Compilar el Proyecto

Configura y compila el proyecto utilizando CMake y Make:

```bash
mkdir build
cd build
cmake .. -DCMAKE_TOOLCHAIN_FILE=./build/Debug/generators/conan_toolchain.cmake -DCMAKE_BUILD_TYPE=Debug -DRUN_COVERAGE=TRUE
make -j$(nproc)
```

### Paso 4: Compilar el programa de monitoreo

Compila el programa de monitoreo ejecutando el bash del mismo

```bash
cd Metrics_Monitoring_System
./build.sh
```

## Ejecutar la Shell

Para ejecutar la shell personalizada, vuelve al directorio raiz y situate en el directorio `build`. A continuación, utiliza el siguiente comando:

```bash
./ShellProject
```

## Uso de la Shell

La shell personalizada soporta los siguientes comandos internos:

- **`cd <directorio>`**: Cambia el directorio actual a `<directorio>`.
- **`clr`**: Limpia la pantalla.
- **`echo <comentario|$env_var>`**: Muestra `<comentario>` en la pantalla.
- **`quit`**: Cierra la shell.
- **`help`**: Muestra los comandos soportados.
- **`start_monitor`**: Inicia el programa de monitoreo.
- **`stop_monitor`**: Detiene el programa de monitoreo.
- **`status_monitor`**: Muestra el estado actual del monitoreo.
- **`show_config`**: Muestra la configuración actual del programa de monitoreo.
- **`update_config <key> <value>`**: Actualiza la configuración del programa de monitoreo.

## Configuración del Programa de Monitoreo

El programa de monitoreo utiliza un archivo de configuración en formato JSON (`config.json`). Puedes modificar este archivo para ajustar las configuraciones del programa de monitoreo, como los intervalos de muestreo y las métricas a recolectar.

## Visualización de las Métricas

El programa de monitoreo expone las métricas en el puerto 8000. En primera instancia, puedes acceder a ellas desde tu navegador web o utilizando `curl`:

```bash
curl http://localhost:8000/metrics
```

### Acceso a las Métricas mediante Grafana

Si tienes Prometheus y Grafana server iniciados, también puedes acceder a las métricas mediante Grafana. Sigue estos pasos:

#### Paso 1: Configura Prometheus:

Asegúrate de que Prometheus esté configurado para recolectar las métricas desde `http://localhost:8000/metrics`.

Añade el siguiente `job` a tu archivo de configuración de Prometheus (`prometheus.yml`):

```yaml
scrape_configs:
    - job_name: "prometheus"
        static_configs:
        - targets: ["localhost:9090"]
    - job_name: "metrics"
        static_configs:
        - targets: ["localhost:8000"]
```

#### Paso 2: Inicia Prometheus:

Inicia Prometheus con tu archivo de configuración:

```bash
prometheus --config.file=prometheus.yml
```

#### Paso 3: Configura Grafana:

Con Grafana instalado y corriendo, abre Grafana en tu navegador (`http://localhost:3000`).

Añade Prometheus como una fuente de datos en Grafana:
        - Ve a **Configuration** > **Data Sources**.
        - Haz clic en **Add data source**.
        - Selecciona **Prometheus**.
        - Configura la URL de Prometheus (`http://localhost:9090`) y guarda la configuración.

#### Paso 4: Accede al Dashboard en Grafana:

Puedes acceder al dashboard existente en Grafana utilizando el siguiente enlace: [Dashboard de Métricas](http://localhost:3000/d/ddznv2haf0ef4c/pc-metrics?orgId=1&from=1731766025448&to=1731766925448)
