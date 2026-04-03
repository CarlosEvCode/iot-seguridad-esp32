#include <Arduino.h>
#include <Wire.h>
#include <Keypad.h>
#include <LiquidCrystal_I2C.h>
#include <ESP32Servo.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>

#define PIN_TRIG 26
#define PIN_ECHO 27
#define PIN_SERVO 13
#define PIN_BUZZER 12
#define PIN_LED 25

const char* WIFI_SSID = "Wokwi-GUEST";
const char* WIFI_PASS = "";
const char* SERVIDOR_MQTT = "broker.hivemq.com";
const int PUERTO_MQTT = 1883;
const char* TOPIC_INTENTOS = "puerta-inteligente/intentos";
const char* TOPIC_PUERTA = "puerta-inteligente/puerta";

char teclas[4][4] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};

byte pinesFilas[4] = {19, 18, 5, 17};
byte pinesColumnas[4] = {16, 4, 2, 15};

Keypad teclado = Keypad(makeKeymap(teclas), pinesFilas, pinesColumnas, 4, 4);
LiquidCrystal_I2C pantalla(0x27, 16, 2);
Servo servo;
WiFiClient clienteWifi;
PubSubClient mqtt(clienteWifi);

String clave = "1234";
String entrada = "";

void conectarWiFi() {
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi conectado");
}

void conectarMQTT() {
  mqtt.setServer(SERVIDOR_MQTT, PUERTO_MQTT);
  while (!mqtt.connected()) {
    if (mqtt.connect("esp32-puerta")) {
      Serial.println("MQTT conectado");
    } else {
      delay(5000);
    }
  }
}

long leerDistancia() {
  digitalWrite(PIN_TRIG, LOW);
  delayMicroseconds(2);
  digitalWrite(PIN_TRIG, HIGH);
  delayMicroseconds(10);
  digitalWrite(PIN_TRIG, LOW);
  return pulseIn(PIN_ECHO, HIGH) * 0.034 / 2;
}

void sonar(int frecuencia, int duracion) {
  int onda = 1000000 / frecuencia;
  long ciclos = (duracion * 1000L) / onda;
  for (long i = 0; i < ciclos; i++) {
    digitalWrite(PIN_BUZZER, HIGH);
    delayMicroseconds(onda / 2);
    digitalWrite(PIN_BUZZER, LOW);
    delayMicroseconds(onda / 2);
  }
}

void enviarEvento(const char* tipo, const char* resultado, const char* estado, int distancia) {
  if (!mqtt.connected()) return;
  
  StaticJsonDocument<128> doc;
  doc["tipo"] = tipo;
  if (resultado) doc["resultado"] = resultado;
  if (estado) doc["estado"] = estado;
  doc["distancia_cm"] = distancia;
  
  char buffer[128];
  serializeJson(doc, buffer);
  
  const char* topic = (strcmp(tipo, "intento") == 0) ? TOPIC_INTENTOS : TOPIC_PUERTA;
  mqtt.publish(topic, buffer);
  Serial.println(buffer);
}

void abrirPuerta() {
  pantalla.clear();
  pantalla.print("ABIERTA");
  sonar(1000, 200);
  enviarEvento("puerta", NULL, "abierta", leerDistancia());
  
  servo.write(90);
  delay(3000);
  
  while (leerDistancia() < 50) delay(200);
  
  pantalla.clear();
  pantalla.print("CERRANDO");
  enviarEvento("puerta", NULL, "cerrada", leerDistancia());
  servo.write(0);
  delay(1000);
}

void denegarAcceso() {
  pantalla.clear();
  pantalla.print("DENEGADO");
  sonar(300, 1000);
  delay(2000);
}

void setup() {
  Serial.begin(115200);
  Wire.begin(21, 22);
  
  pantalla.init();
  pantalla.backlight();
  pantalla.print("INICIANDO");
  
  pinMode(PIN_TRIG, OUTPUT);
  pinMode(PIN_ECHO, INPUT);
  pinMode(PIN_LED, OUTPUT);
  pinMode(PIN_BUZZER, OUTPUT);
  
  servo.setPeriodHertz(50);
  servo.attach(PIN_SERVO, 500, 2400);
  servo.write(0);
  
  conectarWiFi();
  conectarMQTT();
  
  pantalla.clear();
  pantalla.print("INGRESA CLAVE:");
}

void loop() {
  if (!mqtt.connected()) conectarMQTT();
  mqtt.loop();
  
  long distancia = leerDistancia();
  digitalWrite(PIN_LED, distancia > 0 && distancia < 30 ? HIGH : LOW);
  
  char tecla = teclado.getKey();
  if (!tecla) return;
  
  if (tecla == '*') {
    entrada = "";
    pantalla.setCursor(0, 1);
    pantalla.print("                ");
  } else if (tecla == '#') {
    bool exito = (entrada == clave);
    enviarEvento("intento", exito ? "exitoso" : "fallido", NULL, distancia);
    
    if (exito) {
      abrirPuerta();
    } else {
      denegarAcceso();
    }
    
    entrada = "";
    pantalla.clear();
    pantalla.print("INGRESA CLAVE:");
  } else {
    entrada += tecla;
    pantalla.setCursor(0, 1);
    pantalla.print(entrada);
  }
}
