// Importar las librerias a utilizar
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h>

// Dirección I2C del display
#define i2c_Address 0x3c //initialize with the I2C addr 0x3C Typically eBay OLED's

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1   // Reset no usado en la mayoría de placas

//Inicializa la configuración de la pantalla
Adafruit_SH1106G display = Adafruit_SH1106G(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Pin del sensor LM35
const int pinLM35 = A0;

// Variables de tiempo para muestreo
const unsigned long intervalo = 1000; // en milisegundos
unsigned long ultimaMillis = 0;

void setup() {
  Serial.begin(9600);
  while (!Serial) { } 
  
  delay(250); 

  // Inicializar OLED
  if (!display.begin(i2c_Address, true)) {
    Serial.println(F("Error: no se detecta pantalla OLED"));
    for (;;); 
  }

  display.setContrast(200); // brillo (0–255)

  // Limpiar display
  display.clearDisplay();
  display.display();

  Serial.println("LM35 + OLED listo");
}

void loop() {
  unsigned long ahora = millis();
  if (ahora - ultimaMillis >= intervalo) {
    ultimaMillis = ahora;

    int lectura = analogRead(pinLM35); // 0..1023
    const float Vref = 5.0; // ajustar si tu referencia ADC es distinta
    float voltaje = lectura * (Vref / 1023.0); // voltaje en V
    float tempC = voltaje * 100.0; // LM35: 10 mV/°C -> factor 100

    // Imprime en formato legible
    Serial.print("Temperatura: ");
    Serial.print(tempC, 2); // 2 decimales
    Serial.println(" °C");

    // Clear the buffer.
    display.clearDisplay();

    // Texto de cabecera
    display.setTextSize(1); // tamaño pequeño
    display.setTextColor(SH110X_WHITE);
    display.setCursor(10, 0);
    display.println("Temperatura LM35");

    // Texto de valor grande
    display.setTextSize(2); // tamaño grande
    display.setTextColor(SH110X_WHITE); // blanco
    display.setCursor(20, 25);
    display.print(tempC, 1);
    display.print((char)223); // símbolo de grados celcius
    display.println("C");

    //Para guardar los cambios hechos en la pantalla
    display.display();
  }
}
