#include <Arduino.h>
const int ledPin = 13; // LED connected to digital pin 13
const int potPin = A4; // Potentiometer connected to analog pin A4
volatile bool ledState = LOW; // LED state flag
void setup() {
pinMode(ledPin, OUTPUT);
pinMode(potPin, INPUT);
// Initialize Timer1 with a default frequency (0.5 Hz: LED toggles every 2 seconds)
startTimer(0.5);
// Enable global interrupts
sei();
}
void loop() {
// Read potentiometer value (0 to 1023) and map it to a desired frequency range.
// For example, map to a range from 0.5 Hz to 2.0 Hz.
int potValue = analogRead(potPin);
double timerFrequency = map(potValue, 0, 1023, 50, 200) / 100.0; // yields 0.5 Hz to 2.0 Hz
// Update the timer frequency based on potentiometer input.
startTimer(timerFrequency);
// Small delay to prevent excessive timer reconfiguration
delay(500);
}
// Timer1 Compare Match A interrupt service routine
ISR(TIMER1_COMPA_vect) {
ledState = !ledState; // Toggle LED state
digitalWrite(ledPin, ledState); // Update LED output
}
void startTimer(double timerFrequency) {
// Stop Timer1 by clearing control register
TCCR1B = 0;
// Calculate the OCR1A value:
// Using a prescaler of 256 and F_CPU = 16,000,000 Hz:
// OCR1A = (F_CPU / (prescaler * timerFrequency)) - 1
uint32_t ocrValue = (16000000UL / (256 * timerFrequency)) - 1;
OCR1A = ocrValue;
// Set Timer1 to CTC mode (Clear Timer on Compare Match) with a prescaler of 256.
// WGM12 bit sets CTC mode; CS12 bit sets prescaler to 256.
TCCR1A = 0;
TCCR1B = (1 << WGM12) | (1 << CS12);
// Enable Timer1 Compare Match A interrupt.
TIMSK1 |= (1 << OCIE1A);
}
