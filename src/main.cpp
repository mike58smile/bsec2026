#include <Arduino.h>
#include <avr/interrupt.h>
#include "pinout.h"
#include "TimedEvent.h"

// ---------- LED toggle callbacks (ISR-safe: only digitalWrite) ----------
void setupTimer2() {
  cli();
  TCCR2A = 0;
  TCCR2B = 0;
  TCNT2  = 0;
  OCR2A  = 249;                   // 16 MHz / 64 / 250 = 1 kHz → 1 ms
  TCCR2A |= (1 << WGM21);        // CTC mode
  TCCR2B |= (1 << CS22);         // prescaler 64
  TIMSK2 |= (1 << OCIE2A);       // enable compare match A interrupt
  sei();
}
volatile bool led1State = false;
volatile bool led2State = false;

void toggleLed1() {
  led1State = !led1State;
  digitalWrite(LED_PIN, led1State);
}

void toggleLed2() {
  led2State = !led2State;
  digitalWrite(LED2_PIN, led2State);
}

// ---------- Two TimedEvent instances ----------
TimedEvent led1_timer(500, toggleLed1);   // LED_PIN  blinks every 500 ms
TimedEvent led2_timer(1000, toggleLed2);  // LED2_PIN blinks every 1000 ms

// =====================================================================
//  Hardware Timer2 — CTC mode, 1 ms interrupt (16 MHz / 64 / 250)
// =====================================================================

// ---------- Single ISR ticks all timers ----------
ISR(TIMER2_COMPA_vect) {
  led1_timer.tick();
  led2_timer.tick();
}

void setup() {
  pinMode(LED_PIN, OUTPUT);
  pinMode(LED2_PIN, OUTPUT);
  setupTimer2();
}

void loop() {
  // Everything is handled by the ISR — nothing to do here
}
