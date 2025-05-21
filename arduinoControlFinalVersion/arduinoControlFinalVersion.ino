#define TRIG_PIN 11  // Pin connected to the Trigger pin of HC-SR04
#define ECHO_PIN 10  // Pin connected to the Echo pin of HC-SR04

int potentiometer = 0;
int RPM = 0;
int LED = 5; // Change LED pin to avoid timer conflicts
int fan = 9;
int sensor = 2; // RPM sensor pin
int fan_control;
volatile int pulseCount = 0;
unsigned long lastMillis = 0;
int distance = 0;

void rpmISR() {
    pulseCount++; // Interrupt service routine for RPM measurement
}

void setup() {
    Serial.begin(115200); // Increase baud rate for faster communication
    pinMode(LED, OUTPUT);
    pinMode(fan, OUTPUT);
    pinMode(sensor, INPUT_PULLUP);
    pinMode(TRIG_PIN, OUTPUT);
    pinMode(ECHO_PIN, INPUT);
    attachInterrupt(digitalPinToInterrupt(sensor), rpmISR, FALLING);

    // Set Timer1 to 25 kHz for pin 9
    TCCR1A = 0;
    TCCR1B = 0;
    TCNT1 = 0;

    TCCR1A |= (1 << WGM11) | (1 << COM1A1);
    TCCR1B |= (1 << WGM13) | (1 << WGM12) | (1 << CS10);

    ICR1 = 640;
}

void loop() {
    static int smoothedPot = 0;
    static unsigned long lastPrint = 0;
    static unsigned long lastUltrasonic = 0;
    unsigned long currentMillis = millis();

    // Read and process potentiometer
    potentiometer = pow(analogRead(A1) / 1023.0, 2.5) * 255;

    // Motor control
    if (potentiometer < 10) {
        OCR1A = 0;
    } else {
        fan_control = map(potentiometer, 0, 255, 0, 1023);
        OCR1A = fan_control;
    }

    analogWrite(LED, potentiometer);

    // RPM calculation every second
    if (currentMillis - lastMillis >= 1000) {
        noInterrupts();
        RPM = pulseCount * 17.7;
        pulseCount = 0;
        lastMillis = currentMillis;
        interrupts();
    }

    // Ultrasonic sensor every 100 ms (adjustable)
    if (currentMillis - lastUltrasonic >= 100) {
        digitalWrite(TRIG_PIN, LOW);
        delayMicroseconds(2);
        digitalWrite(TRIG_PIN, HIGH);
        delayMicroseconds(10);
        digitalWrite(TRIG_PIN, LOW);

        long duration = pulseIn(ECHO_PIN, HIGH, 20000);  // Optional timeout
        distance = (int)(duration * 0.034 / 2);
        lastUltrasonic = currentMillis;
    }

    // Print to Serial every 200 ms (adjustable)
    if (currentMillis - lastPrint >= 1000) {
        Serial.print("Pot:");
        Serial.print(potentiometer);
        Serial.print("\tRPM:");
        Serial.print(RPM);
        Serial.print(" Distance:");
        Serial.print(distance);
        Serial.println(" cm");
        lastPrint = currentMillis;
    }
}
