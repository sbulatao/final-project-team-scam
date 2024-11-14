const unsigned int LED_PIN = 17;
const unsigned int MOTOR_HIGH_PIN = 13;
const unsigned int MOTOR_LOW_PIN = 14;

void setup() {
    // Initialize the LED pin
    pinMode(LED_PIN, OUTPUT);
    
    // Initialize motor pins
    pinMode(MOTOR_HIGH_PIN, OUTPUT);
    pinMode(MOTOR_LOW_PIN, OUTPUT);
}

void loop() {
    // Spin motor in one direction only at full speed
    blinkAndSpin(MOTOR_HIGH_PIN, MOTOR_LOW_PIN);
}

void blinkAndSpin(unsigned int pin1, unsigned int pin2) {
    // Start the motor at full speed for stronger spinning
    int speed = 255;
    
    // Blink the LED
    digitalWrite(LED_PIN, HIGH);
    
    // Set motor to full speed in one direction
    analogWrite(pin1, speed);
    analogWrite(pin2, 0);
    
    // Adjust delay as needed for faster looping and stronger motor action
    delay(1000); // Keeps motor spinning at full speed for 1 second

    // Turn off LED and stop motor briefly
    digitalWrite(LED_PIN, LOW);
    analogWrite(pin1, 0);
    delay(0);
}
