// #include "DFRobot_PH.h"
// #include <EEPROM.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <ESP32Servo.h> // make a class XD and analog write
// #include <Firebase_ESP_Client.h>
#include <FirebaseESP32.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
// #include <time.h>

// Wi-Fi Configuration
// school
// #define WIFI_SSID "UCSD-PROTECTED"
// #define WIFI_USERNAME "sbulatao" 
// #define WIFI_PASSWORD "Bluemoon1sqxst*"
// #define NON_ENTERPRISE_WIFI_PASSWORD ""
// #define UCSD_PID "A17239438"
// home
#define WIFI_SSID "NETGEAR68"
#define WIFI_PASSWORD "melodicbird404"

// Firebase Configuration
#define FIREBASE_HOST "https://ece-196scamproject-default-rtdb.firebaseio.com/"
#define FIREBASE_AUTH "VqdCfSFN2HIRhih1ZGz8stDUESx4bc7i3w6MNGMB"

// Server Configuration
#define SERVER_URL "http://localhost:6543/pHSensorData/plot"

// OLED Configuration
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Pin Definitions
#define OLED_SCL 38
#define OLED_SDA 39
#define PH_SENSOR_PIN 16
#define PUMP_PIN 17
#define SERVO_PIN 15

// Global Variables
FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;
float temperature = 25.0;  // Fixed temperature for simplicity
// DFRobot_PH phSensor;       // pH Sensor instance
Servo myServo;             // Servo instance
unsigned long lastSendTime = 0;

void connectToWiFi() {
    // Serial.print("Connecting to Wi-Fi");
    // WiFi.begin(WIFI_SSID, WIFI_USERNAME, WIFI_PASSWORD); // school
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD); // home
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        // Serial.print(".");
    }
    Serial.println("\nConnected to Wi-Fi.");
    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP());
}

void initializeFirebase() {
    config.host = FIREBASE_HOST;
    config.signer.tokens.legacy_token = FIREBASE_AUTH;

    Firebase.begin(&config, &auth);
    Firebase.reconnectWiFi(true);

    if (Firebase.ready()) {
        Serial.println("Firebase initialized successfully!");
    } else {
        Serial.println("Failed to initialize Firebase.");
    }
}

String getFormattedTime() {
    time_t now = time(nullptr);
    struct tm* timeinfo = localtime(&now);
    char buffer[20];
    strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", timeinfo);
    return String(buffer);
}


void sendDataToFirebase(float pH, String timestamp) {
    String counterPath = "/pHSensors/entryCount"; // Path to store the counter
    String entryPath;

    // Retrieve the current entry count from Firebase
    int entryCount = 0;
    if (Firebase.getInt(fbdo, counterPath)) {
        entryCount = fbdo.intData();
        Serial.print("Current entry count: ");
        Serial.println(entryCount);
    } else {
        Serial.println("Failed to retrieve entry count. Initializing to 0.");
    }

    // Reset the counter to 1 if it exceeds 20
    if (entryCount >= 20) {
        entryCount = 0; // Reset the counter
    }

    // Increment the entry count
    entryCount++;
    entryPath = "/pHSensors/entry" + String(entryCount); // Path for the new entry

    // Create a JSON object for the new entry
    FirebaseJson json;
    json.add("timestamp", timestamp);
    json.add("pH", pH);

    // Overwrite the data at the calculated entry path
    if (Firebase.setJSON(fbdo, entryPath, json)) {
        Serial.print("Data successfully stored at ");
        Serial.println(entryPath);
    } else {
        Serial.print("Error storing data: ");
        Serial.println(fbdo.errorReason());
    }

    // Update the counter in Firebase
    if (Firebase.setInt(fbdo, counterPath, entryCount)) {
        Serial.println("Entry counter updated successfully.");
    } else {
        Serial.print("Failed to update entry counter: ");
        Serial.println(fbdo.errorReason());
    }
}



void sendDataToServer(float pH, String timestamp) {
    if (WiFi.status() == WL_CONNECTED) {
        HTTPClient http;

        // Prepare JSON payload
        StaticJsonDocument<200> jsonDoc;
        jsonDoc["timestamp"] = timestamp;
        jsonDoc["pH"] = pH;

        String jsonPayload;
        serializeJson(jsonDoc, jsonPayload);

        http.begin(SERVER_URL); // Specify the endpoint
        http.addHeader("Content-Type", "application/json"); // Set content type to JSON

        int httpResponseCode = http.POST(jsonPayload); // Send the request

        if (httpResponseCode > 0) {
            // Serial.print("Server Response: ");
            Serial.println(http.getString());
        } else {
            Serial.print("Error sending POST request: ");
            Serial.println(httpResponseCode);
        }

        http.end(); // Close connection
    } else {
        Serial.println("Wi-Fi Disconnected. Cannot send data.");
    }
}

void setup() {
    // Initialize Serial Monitor
    Serial.begin(115200);
    connectToWiFi();

    // Initialize Firebase
    initializeFirebase();

    // Initialize EEPROM for calibration data storage
    //EEPROM.begin(512);

    // Initialize pH Sensor
    // phSensor.begin();
    // Serial.println("pH Sensor initialized. Calibrate with known buffer solutions if needed.");

    // Initialize I2C for OLED
    Wire.begin(OLED_SDA, OLED_SCL);

    // Initialize OLED Display
    if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
        // Serial.println(F("OLED initialization failed!"));
        for (;;);  // Halt execution
    }
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(0, 0);
    // display.println(F("OLED Initialized"));
    display.display();

    // Configure ADC
    analogReadResolution(12);          // Set ADC resolution to 12 bits
    analogSetAttenuation(ADC_11db);    // Set attenuation for full input range (0-3.3V)

    // Initialize Pump
    pinMode(PUMP_PIN, OUTPUT);
    digitalWrite(PUMP_PIN, LOW);  // Ensure pump is OFF initially

    // Initialize Servo
    myServo.attach(SERVO_PIN);
    myServo.write(90); // Set servo to the neutral position (90 degrees)

    // Synchronize Time via NTP
    configTime(0, 0, "pool.ntp.org", "time.nist.gov");
    // Serial.println("Synchronizing time...");
    while (time(nullptr) < 1000) { // Wait until time is synchronized
        delay(500);
        // Serial.print(".");
    }
    // Serial.println(" Time synchronized.");
}

void loop() {
    // Read pH sensor data
    int pHValue = analogRead(PH_SENSOR_PIN);
    float voltage = pHValue * (5 / 4095.0); // Convert ADC value to voltage (3.3V reference)
    float pH = 9 + ((2.5 - voltage) / 0.18); // Calculate pH value

    // Print results to Serial Monitor
    Serial.print("Raw ADC Value: ");
    Serial.print(pHValue);
    Serial.print(" | Voltage: ");
    Serial.print(voltage, 2);
    Serial.print("V | pH: ");
    Serial.println(pH);

    // Display pH on OLED
    display.clearDisplay();
    display.setCursor(0, 0);
    display.println(F("pH Sensor Reading:"));
    display.print(F("pH: "));
    display.println(pH, 2);
    // display.display();

    // Send data to Firebase and the server every 5 seconds
    if (millis() - lastSendTime > 5000) {
        lastSendTime = millis();
        String timestamp = getFormattedTime();
        sendDataToFirebase(pH, timestamp);
        // sendDataToServer(pH, timestamp);
    }

    String pumpStatus;
    String servoStatus;

  // Pump and Servo Control
  if (pH < 6.5 || pH > 8) {
    Serial.println("pH is out of range.");
    // Serial.println("Activating pump and moving servo...");
    digitalWrite(PUMP_PIN, HIGH);   // Turn on pump
    pumpStatus = "ON";
    servoStatus = "MOVING";
    // Oscillate the servo between 0° and 180°
    for (int angle = 0; angle <= 180; angle += 10) {
      myServo.write(angle);
      delay(100);  // Adjust speed of movement HERE
    }
    for (int angle = 180; angle >= 0; angle -= 10) {
      myServo.write(angle);
      delay(100);  // Adjust speed of movement HERE
    }
  } else {
    Serial.println("pH is within range.");
    // Serial.println("Turning off pump and resetting servo...");
    digitalWrite(PUMP_PIN, LOW);// Turn off pump
    pumpStatus = "OFF";
    servoStatus = "STOPPED";
    myServo.write(0); // Reset servo
  }
  // Display pump and servo status
  display.setCursor(0,20);
  display.print(F("Pump: "));
  display.println(pumpStatus);
  display.print(F("Servo: "));
  display.print(servoStatus);
  display.display();

  // Calibration (Optional, only when calibrating with buffer solutions)
  // phSensor.calibration(voltage, temperature);  // Uncomment this line for calibration

  delay(1000); // Delay before next reading
}
