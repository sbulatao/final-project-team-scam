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
    // Blink and spin motor in one direction
    blinkAndSpin(MOTOR_HIGH_PIN, MOTOR_LOW_PIN);

    // Blink and spin motor in the opposite direction
    // blinkAndSpin(MOTOR_LOW_PIN, MOTOR_HIGH_PIN);
}

void blinkAndSpin(unsigned int pin1, unsigned int pin2) {
    for (int speed = 0; speed <= 255; speed += 5) {
        // Blink the LED every 25 increments of speed
        digitalWrite(LED_PIN, (speed % 25 == 0) ? HIGH : LOW);

        // Control the motor speed
        analogWrite(pin1, speed);
        analogWrite(pin2, 0);

        // Delay to control the speed of the loop
        delay(50);
    }
}