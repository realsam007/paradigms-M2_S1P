// C++ code

#include <avr/io.h>
#include <avr/interrupt.h>

const byte LED_PIN = 13;
const byte METER_PIN = A4;

volatile double timerFrequency = 0.5;  // Default to 0.5Hz (2s interval)

void setup() {
  pinMode(LED_PIN, OUTPUT);
  pinMode(METER_PIN, INPUT);
  
  Serial.begin(9600);

  startTimer(timerFrequency);
}

void loop() {
  // Read potentiometer and adjust timer frequency dynamically
  int sensorValue = analogRead(METER_PIN);
  double newFrequency = map(sensorValue, 0, 1023, 1, 10) / 10.0; // Maps to 0.1Hz - 1Hz (10s - 1s interval)

  if (newFrequency != timerFrequency) {
    timerFrequency = newFrequency;
    Serial.print("New Timer Frequency: ");
    Serial.print(timerFrequency);
    Serial.println(" Hz");
    startTimer(timerFrequency);
  }

  delay(500);  // Adjust every 500ms
}

void startTimer(double timerFrequency) {
  noInterrupts(); // Disable interrupts during setup
  
  TCCR1A = 0;  // Reset Timer1 control registers
  TCCR1B = 0;
  TCNT1 = 0;   // Reset Timer counter

  // Calculate OCR1A for desired frequency (prescaler 1024)
  // Formula: OCR1A = (16,000,000 / (prescaler * frequency)) - 1
  long ocrValue = (16000000 / (1024 * timerFrequency)) - 1;

  OCR1A = ocrValue;
  TCCR1B |= (1 << WGM12);  // CTC mode
  TCCR1B |= (1 << CS12) | (1 << CS10);  // Prescaler 1024
  TIMSK1 |= (1 << OCIE1A); // Enable Timer Compare Interrupt
  
  interrupts(); // Re-enable interrupts
}

ISR(TIMER1_COMPA_vect) {
  digitalWrite(LED_PIN, digitalRead(LED_PIN) ^ 1); // Toggle LED

  if (digitalRead(LED_PIN) == HIGH) {
    Serial.println("LED ON");
  }
}
