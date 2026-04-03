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
