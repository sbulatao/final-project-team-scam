#include "DFRobot_PH.h"
#include <EEPROM.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// OLED Configuration
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Pin Definitions
#define OLED_SCL 38
#define OLED_SDA 39
#define PH_SENSOR_PIN 16 //26  // Analog pin connected to the pH sensor
#define PUMP_PIN 17

float temperature = 25.0;  // Fixed temperature for simplicity
DFRobot_PH phSensor;       // Create an instance of the pH sensor library

void setup() {
  // Initialize Serial Monitor
  Serial.begin(115200);
  delay(100);

  // Initialize EEPROM for calibration data storage
  EEPROM.begin(512);
  // _neutralVoltage:1500.00 -- 2.5V pH 7
  // _acidVoltage:2032.44 -- 2.2V pH 4

  // Initialize pH Sensor
  phSensor.begin();
  Serial.println("pH Sensor initialized. Calibrate with known buffer solutions if needed.");

  // Initialize I2C for OLED
  Wire.begin(OLED_SDA, OLED_SCL);

  // Initialize OLED Display
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("OLED initialization failed!"));
    for (;;);  // Halt execution
  }
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println(F("OLED Initialized"));
  display.display();

  // Configure ADC
  analogReadResolution(12);          // Set ADC resolution to 12 bits
  analogSetAttenuation(ADC_11db);    // Set attenuation for full input range (0-3.3V)

  // Initialize Pump
  pinMode(PUMP_PIN, OUTPUT);
  digitalWrite(PUMP_PIN, LOW);  // Ensure pump is OFF initially
}

void loop() {
  // Read raw analog value from pH sensor
  int pHValue = analogRead(PH_SENSOR_PIN);

  // Convert ADC value to voltage
  // float voltage = pHValue * (3.3 / 4095.0);  // Using 3.3V reference
  float voltage = pHValue * (5 / 4095.0);

  // Read the pH value using the DFRobot_PH library
  // float pH = phSensor.readPH(voltage, temperature);
  // float pH = 7 + ((2.5 - voltage) / 0.18);  // Assuming a typical pH probe equation
  // 2.5 -- Neutral pH 7 voltage (in volts)
  // 0.18; -- Voltage change per pH unit
  float pH = 9 + ((2.5 - voltage) / 0.18);


  // Print results to Serial Monitor
  Serial.print("Raw ADC Value: ");
  Serial.print(pHValue);
  Serial.print(" | Voltage: ");
  Serial.print(voltage, 2);  // 2 decimal places
  Serial.print("V | pH: ");
  Serial.println(pH);

  // Display pH on OLED
  display.clearDisplay();
  display.setCursor(0, 0);
  display.println(F("pH Sensor Reading:"));
  display.print(F("pH: "));
  display.println(pH, 2);  // 2 decimal places
  display.display();

  // Pump Control
  if (pH < 6.5 || pH > 8) {
    Serial.println("pH is out of range. Activating pump...");
    digitalWrite(PUMP_PIN, HIGH);
  } else {
    Serial.println("pH is within range. Turning off pump...");
    digitalWrite(PUMP_PIN, LOW);
  }

  // Calibration (Optional, only when calibrating with buffer solutions)
  phSensor.calibration(voltage, temperature);  // Uncomment this line for calibration

  delay(1000);  // Delay between readings
}
