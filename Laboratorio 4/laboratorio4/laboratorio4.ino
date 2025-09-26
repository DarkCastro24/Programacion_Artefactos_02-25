#include <ESP8266WiFi.h>
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"

/************************* WiFi Access Point *********************************/
#define WLAN_SSID "ARTEFACTOS"
#define WLAN_PASS "ARTEFACTOS"

/************************* Adafruit.io Setup *********************************/
#define AIO_SERVER "io.adafruit.com"
#define AIO_SERVERPORT 1883
#define AIO_USERNAME  "********"
#define AIO_KEY       "*********"

/************ Global State ***************************************************/
WiFiClient client;
Adafruit_MQTT_Client mqtt(&client, AIO_SERVER, AIO_SERVERPORT, AIO_USERNAME, AIO_USERNAME, AIO_KEY);

/****************************** Feeds ****************************************/
// Feed de Adafruit IO donde se mandan los datos del sensor
Adafruit_MQTT_Publish photocell = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/laboratorio4");

/******************* Variables globales **************************************/
float valor = 0.0;
float voltaje = 0.0;
#define LDR D5
/*************************** Callbacks ***************************************/
void slidercallback(double x) {
  Serial.print("Hey we're in a slider callback, the slider value is: ");
  Serial.println(x);
}

Adafruit_MQTT_Subscribe slider = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/laboratorio4", MQTT_QOS_1);

/*********************************************** SetUp *****************************/
void setup() {
  Serial.begin(115200);
  delay(10);

  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(WLAN_SSID);

  WiFi.begin(WLAN_SSID, WLAN_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();
  Serial.println("WiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  slider.setCallback(slidercallback);
  mqtt.subscribe(&slider);
}

/************************************************ Loop *****************************/
void loop() {
  MQTT_connect();
  mqtt.processPackets(10000);

  if (!mqtt.ping()) {
    mqtt.disconnect();
  }

  // Leer valor del LDR
  valor = analogRead(LDR);   
  voltaje = (valor / 1023.0) * 3.3;  

  Serial.print("Voltaje medido: ");
  Serial.println(valor);

  if (!photocell.publish(valor)) {
    Serial.println(F("Publicación Fallida!"));
  } else {
    Serial.println(F("Publicación Exitosa!"));
  }

  delay(1000); // esperar 3 segundos
}

/******************** Función de conexión MQTT ************************************/
void MQTT_connect() {
  int8_t ret;
  if (mqtt.connected()) return;

  Serial.print("Connecting to MQTT... ");
  uint8_t retries = 3;
  while ((ret = mqtt.connect()) != 0) {
    Serial.println(mqtt.connectErrorString(ret));
    Serial.println("Retrying MQTT connection in 10 seconds...");
    mqtt.disconnect();
    delay(10000);
    retries--;
    if (retries == 0) {
      while (1); // esperar reset
    }
  }
  Serial.println("MQTT Connected!");
}