
// NEED to INSTALL FirebaseESP32-S3: https://github.com/mobizt/Firebase-ESP32
// CONFIGURE the ESP32-S3 to CONNECT to your Firebase database

// MIGHT NEED a .env file

#include <WiFi.h>
#include <FirebaseESP32.h>

// Replace with your WiFi credentials
#define WIFI_SSID "NETGEAR68-5G"
#define WIFI_PASSWORD "melodicbird404"

// Firebase project credentials
#define FIREBASE_HOST "https://ece-196scamproject-default-rtdb.firebaseio.com/"  // Firebase Realtime Database URL
#define FIREBASE_AUTH "VqdCfSFN2HIRhih1ZGz8stDUESx4bc7i3w6MNGMB"  // Firebase database secret

// Firebase and WiFi objects
FirebaseData firebaseData;

void setup() {
  Serial.begin(115200);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  // Connect to WiFi
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConnected to Wi-Fi");

  // Initialize Firebase
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  Firebase.reconnectWiFi(true);

  // Test Firebase connection
  if (Firebase.setString(firebaseData, "/test/message", "ESP32 connected!")) {
    Serial.println("Firebase connected successfully!");
  } else {
    Serial.println("Firebase connection failed: " + firebaseData.errorReason());
  }
}

void loop() {
  // Simulate pH sensor data
  float phValue = random(60, 80) / 10.0;  // Random value between 6.0 and 8.0
  String timestamp = String(millis() / 1000);  // Use timestamp as seconds since boot

  // Push data to Firebase
  if (Firebase.pushJSON(firebaseData, "/pHSensors", "{\"timestamp\":\"" + timestamp + "\",\"value\":" + String(phValue) + "}")) {
    Serial.println("Data pushed successfully:");
    Serial.println("Timestamp: " + timestamp);
    Serial.println("Value: " + String(phValue));
  } else {
    Serial.println("Failed to push data: " + firebaseData.errorReason());
  }

  delay(5000);  // Send data every 5 seconds
}
