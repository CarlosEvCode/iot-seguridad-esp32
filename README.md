# Sistema de Puerta Inteligente con ESP32

Sistema IoT de control de acceso que detecta proximidad, valida contraseña numérica y registra eventos. Combina simulación Wokwi, MQTT, Node-RED, MongoDB y dashboard web.

## 1. Arquitectura

```
ESP32 (Wokwi Simulator)
    ↓ MQTT
broker.hivemq.com
    ↓
Node-RED → MongoDB
    ↓
Backend API (:3000)
    ↓
Frontend Dashboard (:8080)
```

**Flujo de datos:** Sensor → ESP32 publica JSON → MQTT → Node-RED agrega timestamp → MongoDB guarda → Backend expone `/api/datos` → Frontend polling cada 2s

## 2. Estructura del Proyecto

```
├── src/main.cpp              # Firmware ESP32 (WiFi, MQTT, sensores)
├── platformio.ini            # Dependencias PlatformIO
├── diagram.json              # Circuito Wokwi
├── wokwi.toml                # Config simulador
├── node-red/flows.json       # Flow MQTT → MongoDB
├── backend/
│   ├── server.js             # API Express /api/datos
│   └── package.json
├── frontend/
│   ├── index.html            # Dashboard
│   ├── app.js                # Polling cada 2s
│   └── style.css
└── .gitignore
```

## 3. Instalación Rápida

### Requisitos
- VSCode + PlatformIO + Wokwi Simulator (extensiones)
- Node.js v14+
- MongoDB (local)
- Node-RED

### Software Setup

**Node.js & npm:**
```bash
node --version  # Verificar instalación
npm --version
```

**Node-RED (global):**
```bash
npm install -g node-red
node-red  # Acceder en http://localhost:1880
```

En Node-RED: Manage Palette → Install `node-red-node-mongodb`

**MongoDB Compass (Gestor Visual)**

Descarga desde: https://www.mongodb.com/try/download/compass

1. Windows: Ejecuta el instalador `.msi` descargado
2. Linux: Descarga `.deb` o `.rpm` según tu distribución
3. Abre MongoDB Compass
4. Conexión por defecto: `mongodb://localhost:27017` (automática)

**MongoDB (Base de Datos)**

**Windows:**
1. Descarga desde: https://www.mongodb.com/try/download/community
2. Selecciona "Windows (.msi)" en la sección "Community Server"
3. Ejecuta el instalador, marca "Install MongoDB as a Service"
4. MongoDB inicia automáticamente
5. Verificar: Abre PowerShell y ejecuta:
```powershell
Get-Service -Name MongoDB
# Debe mostrar: Status = Running
```

**Linux (Ubuntu/Debian):**
```bash
sudo apt-get update
sudo apt-get install mongodb

# Iniciar servicio
sudo systemctl start mongodb

# Verificar
sudo systemctl status mongodb
# Debe mostrar: active (running)
```

**Crear Base de Datos**

MongoDB la crea automáticamente cuando Node-RED inserta el primer documento. Luego verifica en MongoDB Compass:

1. Abre MongoDB Compass
2. Busca la base de datos: `puerta_inteligente`
3. Abre la colección: `eventos_puerta`
4. Debes ver documentos llegando en tiempo real (cuando Node-RED esté corriendo)

## 4. Compilación y Simulación

**Compilar firmware (Ctrl+Shift+B en VSCode)**

El archivo `platformio.ini` descarga automáticamente las librerías necesarias:

```ini
lib_deps =
  marcoschwartz/LiquidCrystal_I2C
  madhephaestus/ESP32Servo
  knolleary/PubSubClient
  bblanchon/ArduinoJson
```

Espera el mensaje "SUCCESS" en la terminal.

**Ejecutar simulador Wokwi**

1. Abre `diagram.json` en VSCode
2. Presiona Shift+F5 (o haz clic en Run)
3. El simulador muestra: ESP32, LCD, sensor ultrasónico, teclado 4x4, LED, Buzzer
4. El código comienza a ejecutarse automáticamente

## 5. Configuración del Sistema

### Node-RED: Importar Flow

1. Abre http://localhost:1880 en el navegador
2. Menú superior derecho (☰) → **Import**
3. Selecciona el archivo `node-red/flows.json`
4. Haz clic en **Import**
5. Presiona el botón azul **Deploy** en la esquina superior derecha

El flow automáticamente:
- Recibe eventos MQTT en `puerta-inteligente/intentos` y `puerta-inteligente/puerta`
- Parsea JSON
- Agrega timestamp
- Guarda en MongoDB colección `eventos_puerta`

**Verificación en Node-RED:**
- La consola inferior NO debe mostrar errores en rojo
- Los nodos deben mostrar un punto verde (conectados)
- El broker MQTT debe mostrar: "Connected" o estado verde

### MongoDB Compass: Verificar Datos

Mientras Node-RED está corriendo y el simulador envía eventos:

1. Abre **MongoDB Compass**
2. Conexión automática a `mongodb://localhost:27017`
3. En el panel izquierdo busca: `puerta_inteligente` (base de datos)
4. Abre la colección: `eventos_puerta`
5. Debes ver documentos con estructura:
   ```json
   {
     "_id": ObjectId("..."),
     "tipo": "intento",
     "resultado": "valido",
     "distancia_cm": 12.5,
     "createdAt": "2026-04-03T19:30:00.000Z"
   }
   ```

Si no ves documentos:
- Verifica que el simulador está corriendo (Serial Monitor mostrando JSON)
- Verifica que Node-RED está corriendo (sin errores)
- Espera 5-10 segundos (el flow tarda en procesar)

### Backend: Inicio y Verificación

**Windows (PowerShell o CMD):**
```powershell
cd backend
npm install
npm start
# o: node server.js
```

**Linux (Terminal):**
```bash
cd backend
npm install
npm start
# o: node server.js
```

Debe mostrar:
```
[EXITO] Conectado exitosamente a MongoDB
[SISTEMA] Servidor corriendo en el puerto 3000
```

**Verificar que funciona:**

Abre una nueva terminal/PowerShell y ejecuta:

Windows (PowerShell):
```powershell
Invoke-WebRequest http://localhost:3000/api/datos
# Debe devolver JSON con los eventos
```

Linux/macOS:
```bash
curl http://localhost:3000/api/datos
# Debe devolver JSON con los eventos
```

Si no devuelve datos:
- Verifica que MongoDB está corriendo
- Verifica que Node-RED ha procesado eventos
- Revisa la consola del backend para errores

### Frontend: Inicio y Verificación

**Windows (PowerShell):**
```powershell
cd frontend
# Opción A: Python 3
python -m http.server 8080

# Opción B: VSCode Live Server (clic derecho en index.html → Open with Live Server)
```

**Linux (Terminal):**
```bash
cd frontend
python3 -m http.server 8080

# O en macOS
python -m http.server 8080
```

Abre en navegador: http://localhost:8080

**Verificar que funciona:**
- La tabla debe mostrar eventos (últimos 10)
- Se actualiza cada 2 segundos (mira el timestamp)
- El indicador de conexión debe estar en VERDE
- Si ves "Conectando..." en rojo, revisa que backend está corriendo

## 6. Inicio Rápido

Ejecuta los siguientes comandos en **terminales separadas** (abre una terminal para cada servicio):

**Terminal 1: Verificar MongoDB está corriendo**

Windows (PowerShell):
```powershell
Get-Service -Name MongoDB
# Debe mostrar: Status = Running
```

Linux:
```bash
sudo systemctl status mongodb
# Debe mostrar: active (running)
```

**Terminal 2: Simulador ESP32**

1. En VSCode, abre el archivo `diagram.json`
2. Presiona **Shift+F5** o haz clic en el botón **Run**
3. El simulador inicia y muestra el Serial Monitor
4. Debes ver JSON siendo publicado cada cierto tiempo:
   ```json
   {"tipo":"intento","resultado":"valido"}
   {"tipo":"puerta","estado":"abierta","distancia_cm":5.2}
   ```

**Terminal 3: Node-RED**

Windows (PowerShell):
```powershell
node-red
# Espera: Node-RED running at http://localhost:1880
```

Linux (Terminal):
```bash
node-red
# Espera: Node-RED running at http://localhost:1880
```

Abre navegador: http://localhost:1880

Importa el flow (ver Sección 5 → Node-RED: Importar Flow)

**Terminal 4: Backend API**

Windows (PowerShell):
```powershell
cd backend
npm install  # Solo la primera vez
node server.js
# Espera: [SISTEMA] Servidor corriendo en el puerto 3000
```

Linux (Terminal):
```bash
cd backend
npm install  # Solo la primera vez
node server.js
# Espera: [SISTEMA] Servidor corriendo en el puerto 3000
```

**Terminal 5: Frontend**

Windows (PowerShell):
```powershell
cd frontend
python -m http.server 8080
# Espera: Serving HTTP on port 8080
```

Linux (Terminal):
```bash
cd frontend
python3 -m http.server 8080
# Espera: Serving HTTP on port 8080
```

Abre navegador: http://localhost:8080

### Checklist Final

| Componente | URL | Estado | Acción |
|-----------|-----|--------|--------|
| MongoDB | http://localhost:27017 | Debe estar corriendo | Ver en Servicios |
| Node-RED | http://localhost:1880 | Debe mostrar flow importado | Verificar en consola (sin rojo) |
| Backend | http://localhost:3000/api/datos | Debe devolver JSON | Prueba en navegador o curl |
| Frontend | http://localhost:8080 | Debe mostrar tabla | Ver indicador en verde |
| MongoDB Compass | Conexión automática | Debe mostrar `puerta_inteligente` | Buscar documentos en `eventos_puerta` |

### Resultado Esperado

Dashboard muestra eventos en vivo
Tabla se actualiza cada 2 segundos
Indicador de conexión está en VERDE
MongoDB Compass muestra documentos llegando
Serial Monitor del simulador muestra JSON enviado
Sin errores en ninguna terminal


## 7. Arquitectura Técnica

```
ESP32 (sensor input)
  ↓
Publica JSON a MQTT
  ↓
broker.hivemq.com recibe
  ↓
Node-RED suscrito a tópicos
  ↓
Agrega timestamp + tipo
  ↓
MongoDB insertOne() en eventos_puerta
  ↓
Frontend polling: GET /api/datos cada 2s
  ↓
Backend query: find().sort(-createdAt).limit(10)
  ↓
JSON response
  ↓
Dashboard UI actualiza
```

### Componentes

| Componente | Rol | Puerto |
|-----------|-----|--------|
| ESP32 | Lectura de sensores, publicación MQTT | - |
| MQTT Broker | Comunicación pub/sub | 1883 |
| Node-RED | Procesamiento y almacenamiento | 1880 |
| MongoDB | Base de datos eventos | 27017 |
| Backend API | HTTP endpoint datos | 3000 |
| Frontend | Dashboard visualización | 8080 |

