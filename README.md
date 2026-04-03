# Sistema de Puerta Inteligente con ESP32

## 1. Introducción

Este proyecto implementa un sistema de control de acceso inteligente utilizando un microcontrolador ESP32. El sistema detecta proximidad mediante un sensor ultrasónico, valida acceso con una contraseña numérica, y registra todos los eventos en una base de datos (MongoDB).

El flujo de datos va desde el dispositivo hasta una base de datos MongoDB, pasando por Node-RED para procesamiento, y finalmente se visualiza en un dashboard web en tiempo real (polling cada 2 segundos).

### Objetivo

Crear un prototipo funcional de sistema de puerta inteligente que demuestre:

- Lectura de sensores en tiempo real
- Validación de acceso
- Procesamiento de eventos
- Almacenamiento en base de datos
- Visualización de datos en dashboard

### Arquitectura General

```
Simulador Wokwi (ESP32)
    ↓
MQTT (broker.hivemq.com)
    ↓
Node-RED (procesamiento)
    ↓
MongoDB (almacenamiento)
    ↓
Backend API (endpoint /api/datos)
    ↓
Frontend (dashboard web)
```

### Requisitos Previos

**Software:**

- VSCode
- Extensión Wokwi Simulator para VSCode
- Extensión PlatformIO para VSCode
- Node.js (v14 o superior)
- MongoDB (instalación local)
- Node-RED

---

## 2. Estructura del Proyecto

```
iot-seguridad-esp32/
├── src/
│   └── main.cpp                 # Código firmware ESP32
├── platformio.ini               # Configuración de compilación y dependencias
├── diagram.json                 # Esquema del circuito para simulador Wokwi
├── wokwi.toml                   # Configuración de comportamiento del simulador
├── node-red/
│   └── flow.json                # Flujo MQTT para procesar eventos q se importa a Node-RED
├── backend/
│   ├── server.js                # API Express con endpoint /api/datos
│   └── package.json             # Dependencias del backend
├── frontend/
│   ├── index.html               # Estructura del dashboard web
│   ├── app.js                   # Lógica de polling y actualización UI
│   └── style.css                # Estilos y diseño del dashboard
└── .gitignore                   # Archivos ignorados en control de versiones
```

### Descripción de Componentes

**src/main.cpp**

- Código principal del ESP32
- Gestiona conexión WiFi y MQTT
- Lee sensores (distancia, teclado)
- Publica eventos a tópicos MQTT
- Variables y funciones

**platformio.ini**

- Define plataforma ESP32
- Lista librerías requeridas (WiFi, MQTT, Sensor, LCD, Keypad)
- Configuración de velocidad de puerto y compilación

**diagram.json y wokwi.toml**

- Definen circuito virtual en simulador Wokwi
- Especifican componentes: ESP32, LCD, sensor ultrasónico, teclado 4x4, LED, Buzzer
- Configuran comportamiento de simulación

**node-red/flow.json**

- Flujo automatizado de procesamiento
- Recibe eventos MQTT de dos tópicos: intentos y puerta
- Agrega timestamp a cada evento
- Guarda eventos en MongoDB

**backend/server.js**

- Servidor Express en puerto 3000
- Endpoint GET /api/datos devuelve últimos 10 eventos
- Se conecta a MongoDB
- Permite CORS para acceso desde frontend

**frontend/**

- Dashboard web con estadísticas en vivo
- Muestra estado de puerta, sensor de distancia, historial
- Se actualiza cada 2 segundos via polling
- Indicador visual de conexión al servidor


## SECCIÓN 3: INSTALACIÓN DE HERRAMIENTAS

### VSCode + Extensión Wokwi Simulator
1. Descarga el instalador desde `code.visualstudio.com`.
2. En la pestaña de Extensiones (Ctrl+Shift+X), busca e instala **Wokwi Simulator**.
3. El proyecto reconocerá automáticamente el archivo `diagram.json` al abrir la carpeta.
4. Ejecuta la simulación abriendo `main.cpp` y presionando `Shift + F5`.

### PlatformIO
1. En Extensiones de VSCode, busca e instala **PlatformIO IDE**.
2. Este framework gestiona automáticamente las librerías y la compilación para el ESP32.
3. Utiliza los iconos de la barra inferior para compilar (Check) y subir el código.

### Node.js y npm
1. Descarga la versión LTS desde `nodejs.org`.
2. Verifica la instalación en terminal:
```bash
node --version
npm --version
```

## SECCIÓN 4: INSTALACIÓN DE NODE-RED
¿Qué es Node-RED y por qué lo usamos?
Node-RED es una herramienta de programación visual que funciona como el "Middleware" del sistema. Se encarga de recibir los eventos MQTT del ESP32, procesarlos y almacenarlos en la base de datos.

Instalación (Windows y Linux)
Abre tu terminal (CMD, PowerShell o Bash) y ejecuta los siguientes comandos:

```bash
# 1. Instalar Node-RED globalmente en el sistema
npm install -g node-red

# 2. Iniciar el servidor
node-red
```

Una vez iniciado, accede a la interfaz gráfica abriendo http://localhost:1880 en tu navegador.

Instalación de nodos de MongoDB
En Node-RED, ve al menú superior derecho (☰) y selecciona Manage Palette.

Ve a la pestaña Install y busca node-red-node-mongodb.

Haz clic en instalar.

Detén el servidor en la terminal (Ctrl+C) y vuelve a ejecutar node-red para aplicar los cambios.

## SECCIÓN 5: INSTALACIÓN DE MONGODB
¿Qué es MongoDB y por qué lo usamos?
Es una base de datos NoSQL que almacena la información en formato JSON nativo, ideal para procesar y guardar directamente los datos IoT generados por Node-RED.

Instalación en Windows
Descarga el instalador desde mongodb.com/try/download/community.

Selecciona la plataforma Windows (formato msi).

Ejecuta el instalador y asegúrate de marcar la opción "Install MongoDB as a Service".

Sigue el asistente para instalar también MongoDB Compass (interfaz visual).

Para verificar que el servicio está activo, abre PowerShell y ejecuta:

```powershell
Get-Service -Name MongoDB
```


Instalación en Linux (Ubuntu/Debian)

Abre tu terminal y ejecuta:

```bash
# Instalar MongoDB desde repositorios oficiales
sudo apt-get install mongodb

# Iniciar el servicio en segundo plano
sudo systemctl start mongodb

# Verificar el estado del servicio
sudo systemctl status mongodb
```

Base de datos del proyecto
Nombre de BD: puerta_inteligente

Colección: eventos_puerta
(La estructura se creará automáticamente cuando Node-RED inserte el primer registro).