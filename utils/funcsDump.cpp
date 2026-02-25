#include <Arduino.h>
#include "pinout.h"
#include "TimedEvent.h"

float angle = 0.0;

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
toggle_led led_toggle(LED2_PIN);

// Example 1: Constructor with period and callback — prints a message every 2 seconds
TimedEvent print_timer(2000, print_something);

// Example 2: Constructor with enabled=false — starts disabled, enabled later
TimedEvent delayed_timer(3000, []() { Serial.println("Delayed timer fired!"); }, false);

// Example 3: Timer whose period will be changed at runtime
TimedEvent adjustable_timer(1000, []() { Serial.println("Adjustable tick"); });

void setup() {
  // put your setup code here, to run once:
  pinMode(LED2_PIN, OUTPUT);
  Serial.begin(115200);
  pinMode(SINE_OUT_PIN, OUTPUT);
  pinMode(LED_PIN, OUTPUT);

  // Example: set() — configure led_timer after default construction
  led_timer.set(500, [](){led_toggle.callback();});

  // Example: enable() — enable delayed_timer after 0 ms (will start firing in loop)
  delayed_timer.enable();

  // Example: setPeriod() — change adjustable_timer from 1000 ms to 750 ms
  adjustable_timer.setPeriod(750);

  // Example: reset() — restart adjustable_timer's countdown from now
  adjustable_timer.reset();

  // Example: getPeriod() — read back the current period
  Serial.print("Adjustable timer period: ");
  Serial.print(adjustable_timer.getPeriod());
  Serial.println(" ms");

  // Example: isEnabled() — check if a timer is active
  Serial.print("LED timer enabled: ");
  Serial.println(led_timer.isEnabled() ? "yes" : "no");

  // Example: disable() — temporarily stop the print_timer
  print_timer.disable();
  Serial.println("print_timer disabled in setup (re-enabled after 10 s in loop)");
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

  // Update all timers — must be called every loop iteration
  led_timer.update();
  print_timer.update();
  delayed_timer.update();
  adjustable_timer.update();

  // Example: re-enable print_timer after 10 seconds
  if (!print_timer.isEnabled() && millis() > 10000) {
    print_timer.enable();
    print_timer.reset();
    Serial.println("print_timer re-enabled after 10 s!");
  }
}
