#include <Wire.h>
#include <Adafruit_INA219.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// ============================================================
//                       PIN CONFIGURATION
// ============================================================

// I2C pins used to communicate with the INA219 sensor and OLED
#define SDA_PIN 33
#define SCL_PIN 32

// UART2 pins used to communicate with the STM32 microcontroller
#define STM32_RX_PIN 16
#define STM32_TX_PIN 17

// UART communication baud rate (must match STM32 USART6)
#define STM32_BAUD 9600

// Relay GPIO pins (update if wired to different pins)
#define RELAY1_PIN 25
#define RELAY2_PIN 26

// ============================================================
//                     UART COMMAND DEFINITIONS
// ============================================================

#define CMD_READ_INA       'R'
#define CMD_TOGGLE_RELAY1  '1'
#define CMD_TOGGLE_RELAY2  '2'

// ============================================================
//                       OLED CONFIGURATION
// ============================================================

#define SCREEN_WIDTH   128
#define SCREEN_HEIGHT  64
#define OLED_RESET     -1
#define SCREEN_ADDRESS 0x3C

// ============================================================
//                    OBJECT INITIALIZATION
// ============================================================

Adafruit_INA219 ina219;
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// ============================================================
//                    SENSOR & RELAY VARIABLES
// ============================================================

float busVoltage   = 0.0;
float shuntVoltage = 0.0;
float current      = 0.0;
float power        = 0.0;

// Relays initialized active HIGH
bool relay1State = HIGH;
bool relay2State = HIGH;

// ============================================================
//                    TIMING CONFIGURATION
// ============================================================

unsigned long lastSensorRead    = 0;
unsigned long lastDisplaySwitch = 0;

const unsigned long SENSOR_INTERVAL  = 4000;
const unsigned long DISPLAY_INTERVAL = 2000;

bool activePage = 0;

// ============================================================
//                     SENSOR READING FUNCTION
// ============================================================

void readSensors() {
  busVoltage   = ina219.getBusVoltage_V();
  shuntVoltage = ina219.getShuntVoltage_mV();
  current      = ina219.getCurrent_mA();
  power        = ina219.getPower_mW();
}

// ============================================================
//                    UART DATA TRANSMISSION
// ============================================================

void sendUARTData() {
  float values[4] = {current, power, busVoltage, shuntVoltage};
  Serial2.write((uint8_t*)values, sizeof(values));
}

// ============================================================
//                   STM32 COMMAND HANDLER
// ============================================================

void checkSTM32Commands() {
  while (Serial2.available() > 0) {
    char cmd = Serial2.read();

    switch (cmd) {
      case CMD_READ_INA:
        sendUARTData();
        break;

      case CMD_TOGGLE_RELAY1:
        relay1State = !relay1State;
        digitalWrite(RELAY1_PIN, relay1State);
        break;

      case CMD_TOGGLE_RELAY2:
        relay2State = !relay2State;
        digitalWrite(RELAY2_PIN, relay2State);
        break;

      default:
        break;
    }
  }
}

// ============================================================
//                     OLED DISPLAY UPDATE
// ============================================================

void updateDisplay() {
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);

  if (activePage == 0) {
    display.setTextSize(1);
    display.setCursor(0, 0);
    display.println("== CURRENT & POWER ==");

    display.setTextSize(2);
    display.setCursor(0, 18);
    display.print("I:");
    display.print(current, 1);
    display.println("mA");

    display.setCursor(0, 42);
    display.print("P:");
    display.print(power, 1);
    display.println("mW");
  } else {
    display.setTextSize(1);
    display.setCursor(0, 0);
    display.println("== VOLTAGE READINGS ==");

    display.setTextSize(2);
    display.setCursor(0, 18);
    display.print("Vbus:");
    display.print(busVoltage, 2);
    display.println("V");

    display.setCursor(0, 42);
    display.print("Vsh:");
    display.print(shuntVoltage, 2);
    display.println("mV");
  }

  display.display();
}

// ============================================================
//                        SETUP & LOOP
// ============================================================

void setup() {
  Serial.begin(115200);

  // Initialize relays to active HIGH
  pinMode(RELAY1_PIN, OUTPUT);
  pinMode(RELAY2_PIN, OUTPUT);
  digitalWrite(RELAY1_PIN, relay1State);
  digitalWrite(RELAY2_PIN, relay2State);

  // Initialize UART2 for STM32 communication
  Serial2.begin(STM32_BAUD, SERIAL_8N1, STM32_RX_PIN, STM32_TX_PIN);

  // Initialize I2C
  Wire.begin(SDA_PIN, SCL_PIN);

  // Initialize INA219
  if (!ina219.begin()) {
    Serial.println("Failed to find INA219 chip");
    while (1) { delay(10); }
  }

  // Initialize OLED
  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println("SSD1306 allocation failed");
    while (1) { delay(10); }
  }

  // Take an initial reading and show it
  readSensors();
  updateDisplay();
}

void loop() {
  unsigned long now = millis();

  // Always check for incoming STM32 commands immediately
  checkSTM32Commands();

  // Periodically update INA219 readings
  if (now - lastSensorRead >= SENSOR_INTERVAL) {
    lastSensorRead = now;
    readSensors();
    updateDisplay();
  }

  // Periodically switch OLED display pages
  if (now - lastDisplaySwitch >= DISPLAY_INTERVAL) {
    lastDisplaySwitch = now;
    activePage = !activePage;
    updateDisplay();
  }
}