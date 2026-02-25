#include <Arduino.h>
#include "pinout.h"

float angle = 0.0;

class TimedEvent
{
public:
  void set_timed_event(unsigned long time, void (*callback)()){
    m_event_period = time;
    event_callback = callback;
  }

  void check_event(){
    if(millis() - event_time >= m_event_period){
      event_time = millis();
      event_callback();
    }
  }

private:
  unsigned long event_time;
  unsigned long m_event_period;
  void (*event_callback)() = nullptr;
};

class toggle_led
{
public:
  toggle_led(uint8_t led_pin){
    m_led_pin = led_pin;
  }

  void callback(){
    m_led_state = !m_led_state;
    digitalWrite(m_led_pin, m_led_state);
  }
private:
  uint8_t m_led_pin;
  bool m_led_state = false;
};

void print_something(){
  Serial.println("Hello World");
}

unsigned long previousMillis = 0;
const unsigned long LED_INTERVAL = 500;  // blink interval in ms
bool ledState = false;

TimedEvent led_timer;
toggle_led led_toggle(12);

void setup() {
  // put your setup code here, to run once:
  pinMode(12, OUTPUT);
  Serial.begin(9600);
  led_timer.set_timed_event(500, [](){led_toggle.callback();});
  Serial.begin(115200);
  pinMode(SINE_OUT_PIN, OUTPUT);
  pinMode(LED_PIN, OUTPUT);
}

void loop() {
  // Non-blocking LED blink
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= LED_INTERVAL) {
    previousMillis = currentMillis;
    ledState = !ledState;
    digitalWrite(LED_PIN, ledState ? HIGH : LOW);
  }

  // Sine wave output
  int value = (int)(127.5 + 127.5 * sin(angle));  // map sin(-1..1) to 0..255
  analogWrite(SINE_OUT_PIN, value);
  angle += 0.0245;          // 2*PI / 256 ≈ 0.0245 rad per step
  if (angle >= TWO_PI) {
    angle -= TWO_PI;
  }

  // Serial print: $sineValue ledState;
  Serial.print("$");
  Serial.print(value);
  Serial.print(" ");
  Serial.print((int)ledState);
  Serial.println(";");

  delay(10);                // 10 ms delay for serial plotter
}
  led_timer.check_event();
  // put your main code here, to run repeatedly:
}
