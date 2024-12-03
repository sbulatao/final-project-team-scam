const unsigned int LED_PIN = 17;
const unsigned int MOTOR1_HIGH_PIN = 13;
const unsigned int MOTOR1_LOW_PIN = 14;
const unsigned int MOTOR2_HIGH_PIN = 11; // New motor (water pump)
const unsigned int MOTOR2_LOW_PIN = 12;  // New motor (water pump)

void setup() {
    // Initialize the LED pin
    pinMode(LED_PIN, OUTPUT);
    
    // Initialize motor pins
    pinMode(MOTOR1_HIGH_PIN, OUTPUT);
    pinMode(MOTOR1_LOW_PIN, OUTPUT);
    pinMode(MOTOR2_HIGH_PIN, OUTPUT); // Water pump motor
    pinMode(MOTOR2_LOW_PIN, OUTPUT);  // Water pump motor
}

void loop() {
    // Spin both motors and blink LED
    blinkAndSpin(MOTOR1_HIGH_PIN, MOTOR1_LOW_PIN);
    blinkAndSpin(MOTOR2_HIGH_PIN, MOTOR2_LOW_PIN);
}

void blinkAndSpin(unsigned int pin1, unsigned int pin2) {
    // Set motor to full speed for stronger spinning
    int speed = 255;
    
    // Blink the LED
    digitalWrite(LED_PIN, HIGH);
    
    // Set motor to full speed in one direction
    analogWrite(pin1, speed);
    analogWrite(pin2, 0);
    
    // Adjust delay as needed for motor runtime
    delay(1000); // Keeps motor spinning at full speed for 1 second

    // Turn off LED and stop motor briefly
    digitalWrite(LED_PIN, LOW);
    analogWrite(pin1, 0);
    delay(200);
}
