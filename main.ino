#include "DFRobot_PH.h"
#include <EEPROM.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <ESP32Servo.h>

// OLED Configuration
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Pin Definitions
#define OLED_SCL 38
#define OLED_SDA 39
#define PH_SENSOR_PIN 16  //26 -- Analog pin connected to the pH sensor
#define PUMP_PIN 17       
#define SERVO_PIN 15       // Pin connected to the servo signal wire

float temperature = 25.0;  // Fixed temperature for simplicity
DFRobot_PH phSensor;       // Create an instance of the pH sensor library
Servo myServo;             // Create a Servo object

void setup() {
  // Initialize Serial Monitor
  Serial.begin(115200);
  delay(100);

  // Initialize EEPROM for calibration data storage
  EEPROM.begin(512);

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

  // Initialize Servo
  myServo.attach(SERVO_PIN);  // Attach the servo to pin 15
  myServo.write(90);          // Set servo to the neutral position (90 degrees)
}

void loop() {
  // Read raw analog value from pH sensor
  int pHValue = analogRead(PH_SENSOR_PIN);

  // Convert ADC value to voltage
  float voltage = pHValue * (5 / 4095.0);  // Using 3.3V reference

  // Calculate pH value
  // float pH = phSensor.readPH(voltage, temperature);
  // float pH = 7 + ((2.5 - voltage) / 0.18);
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

  String pumpStatus;
  int servoPosition;

  // Pump and Servo Control
  if (pH < 6.5 || pH > 8) {
    Serial.println("pH is out of range.");
    Serial.println("Activating pump and moving servo...");
    digitalWrite(PUMP_PIN, HIGH);   // Turn on pump
    pumpStatus = "ON";

    // myServo.write(180);            // Move servo to 180 degrees
    // Serial.println("Servo moved 180 Degrees");

    // Oscillate the servo between 0° and 180°
    for (int angle = 0; angle <= 180; angle += 10) {
      myServo.write(angle);
      servoPosition = angle;
      delay(100);  // Adjust speed of movement HERE
    }
    for (int angle = 180; angle >= 0; angle -= 10) {
      myServo.write(angle);
      servoPosition = angle;
      delay(100);  // Adjust speed of movement HERE
    }

  } else {
    Serial.println("pH is within range.");
    Serial.println("Turning off pump and resetting servo...");
    digitalWrite(PUMP_PIN, LOW);// Turn off pump
    pumpStatus = "OFF";

    myServo.write(0); // Reset servo
    servoPosition = 0;
    Serial.println("Servo moved 0 Degrees");
  }

  // Display pump and servo status
  display.setCursor(0,20);
  display.print(F("Pump: "));
  display.println(pumpStatus);
  display.print(F("Servo: "));
  display.print(servoPosition);
  display.print(F(" Degrees"));
  display.display();

  // Calibration (Optional, only when calibrating with buffer solutions)
  // phSensor.calibration(voltage, temperature);  // Uncomment this line for calibration

  delay(1000);  // Delay between readings
}
