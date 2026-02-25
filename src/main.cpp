#include <Arduino.h>

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

// put function declarations here:
int myFunction(int, int);

TimedEvent led_timer;
toggle_led led_toggle(12);

void setup() {
  // put your setup code here, to run once:
  pinMode(12, OUTPUT);
  led_timer.set_timed_event(1000, print_something);
}

void loop() {
  led_timer.check_event();
  // put your main code here, to run repeatedly:
}
