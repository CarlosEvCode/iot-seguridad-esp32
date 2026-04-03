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

## 6. Configuración de Wokwi Simulator

### ¿Qué es Wokwi?

Wokwi es un simulador de circuitos electrónicos integrado en VSCode. Permite probar el código del ESP32 sin necesidad de hardware físico, ideal para desarrollo y pruebas rápidas.

### Archivos de Configuración

**diagram.json**

- Define la estructura del circuito virtual
- Especifica qué componentes se utilizan y sus conexiones

**wokwi.toml**

- Archivo de configuración del simulador
- Controla velocidad de ejecución y comportamiento general
- Personalización de cómo se ejecuta la simulación

### Cómo Ejecutar la Simulación

1. Abrir el archivo `diagram.json` en VSCode
2. Se abre automáticamente la ventana del simulador Wokwi
3. Hacer clic en el botón **Run** (botón verde de reproducción)
4. El simulador comienza a ejecutar el código
5. Interactuar con los componentes (presionar botones, ver pantalla LCD, etc)

### Componentes en el Proyecto

El simulador incluye:

- **ESP32 microcontroller** - Cerebro del sistema
- **Pantalla LCD** - Muestra mensajes de estado
- **Sensor ultrasónico** - Detecta proximidad
- **Teclado 4x4** - Permite ingresar contraseña
- **LED** - Indicador visual de estado
- **Buzzer** - Indicador sonoro

## SECCIÓN 7: COMPILACIÓN DEL FIRMWARE

### Proceso de Compilación

El código fuente reside en la ruta `src/main.cpp`. Se utiliza el entorno de PlatformIO para compilar el firmware que será ejecutado por el ESP32.

### Configuración de platformio.ini

Este archivo administra el entorno y descarga las dependencias automáticamente. Asegúrate de que contenga la siguiente estructura:

```ini
[env:esp32dev]
platform = espressif32
board = esp32dev
framework = arduino
lib_deps =
  marcoschwartz/LiquidCrystal_I2C
  madhephaestus/ESP32Servo
  knolleary/PubSubClient
  bblanchon/ArduinoJson
```

### Pasos para Compilar

1. Abre la carpeta del proyecto en VSCode.
2. Espera a que la extensión de PlatformIO se inicialice.
3. Ejecuta la compilación con el atajo de teclado:

```bash
Ctrl + Shift + B
```

4. La terminal descargará las librerías necesarias y mostrará un mensaje de SUCCESS.

### Librerías del Firmware

- **WiFi.h** - Gestión de conexión de red.
- **PubSubClient** - Comunicación mediante protocolo MQTT.
- **LiquidCrystal_I2C** - Control del display alfanumérico.
- **NewPing** - Optimización del sensor ultrasónico.
- **Keypad** - Mapeo y lectura de la matriz de botones físicos.

## 8. Configuración de Node-RED

### Flujo de Datos General

Node-RED recibe eventos MQTT del ESP32 y los procesa para guardarlos en MongoDB:

1. **Entrada** - Recibe mensajes MQTT en dos tópicos:
   - `puerta-inteligente/intentos` - Eventos de validación de acceso
   - `puerta-inteligente/puerta` - Eventos de estado de la puerta

2. **Procesamiento** - Transforma los datos:
   - Parsea JSON recibido
   - Agrega timestamp automáticamente
   - Estructura uniforme del documento

3. **Almacenamiento** - Guarda en MongoDB:
   - Base de datos: `puerta_inteligente`
   - Colección: `eventos_puerta`
   - Documento contiene: tipo, resultado/estado, distancia_cm, createdAt

4. **Resultado** - Evento guardado con fecha de registro

### Importar el Flow al Proyecto

1. Abrir Node-RED en navegador: `http://localhost:1880`
2. Hacer clic en el botón hamburguesa (☰)
3. Seleccionar **Import**
4. Seleccionar archivo: `node-red/flow.json`
5. Hacer clic en **Import**
6. Hacer clic en el botón **Deploy**

### Configurar Conexión MQTT

El flow utiliza la siguiente configuración MQTT (automática):

- **Broker:** broker.hivemq.com
- **Puerto:** 1883
- **Conexión:** Automática al iniciar Node-RED

No requiere configuración manual. Se conecta automáticamente.

### Configurar Conexión MongoDB

1. En la ventana de Node-RED, hacer clic derecho sobre el nodo **MongoDB Local**
2. Seleccionar **Edit**
3. Verificar la configuración:
   - **URL:** `mongodb://localhost:27017`
   - **Database:** `puerta_inteligente`
   - **Collection:** `eventos_puerta`
4. Hacer clic en **Done** para guardar

### Verificación

Después de importar y configurar:

- La consola de Node-RED (abajo) no debe mostrar errores en rojo
- MongoDB Compass debe mostrar documentos llegando en la colección `eventos_puerta`
- El flow debe indicar estado "connected" en los nodos MQTT y MongoDB

## SECCIÓN 9: CONFIGURACIÓN DEL BACKEND

### Descripción
El backend es una API REST construida con Node.js y Express. Su función principal es actuar como un intermediario seguro entre la base de datos MongoDB y la interfaz de usuario, garantizando que no se expongan conexiones directas ni credenciales de base de datos en el cliente.

### Instalación y Ejecución
Abre tu terminal, navega hasta la carpeta `backend/` del proyecto y ejecuta los siguientes comandos:

```bash
# 1. Instalar las dependencias (express, mongodb, cors)
npm install

# 2. Iniciar el servidor API
node server.js
```

Si la conexión es exitosa, la consola mostrará:

```
[EXITO] Conectado exitosamente a MongoDB
[SISTEMA] Servidor corriendo en el puerto 3000
```

### Rutas de la API

**Endpoint: GET /api/datos**

**Función:** Recupera los últimos 10 eventos ordenados de forma descendente (del más reciente al más antiguo) y los devuelven en formato JSON con políticas CORS habilitadas para evitar bloqueos del navegador.

## SECCIÓN 10: CONFIGURACIÓN DEL FRONTEND

### Descripción

El dashboard es una aplicación web dinámica (SPA) construida en HTML, CSS y Vanilla JavaScript. Muestra el estado actual de los sensores y el historial de eventos solicitando los datos al backend de forma asíncrona.

### Ejecución de la Interfaz

El frontend realiza peticiones locales mediante la función Fetch, por lo que debe ser ejecutado a través de un servidor HTTP local.

**Opción A: En Windows (VSCode)**

1. Instala la extensión Live Server en VSCode.
2. Haz clic derecho sobre el archivo `index.html`.
3. Selecciona "Open with Live Server".

**Opción B: Por línea de comandos (Python)**

Abre una terminal en la carpeta `frontend/` y ejecuta el servidor nativo de Python:

```bash
# Para Linux / macOS
python3 -m http.server 8080

# Para Windows
python -m http.server 8080
```

Una vez ejecutado el comando, abre tu navegador web y dirígete a: `http://localhost:8080`

### Lógica de Actualización (Polling)

Para mantener la información en tiempo real sin saturar la red, la aplicación no requiere recargar la página. El archivo `app.js` contiene un temporizador interno que consulta al servidor de forma silenciosa cada cierto tiempo:

```javascript
// El panel web se actualiza automáticamente cada 2 segundos
setInterval(refrescarDatos, 2000);
```