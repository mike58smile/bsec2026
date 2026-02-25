#ifndef TIMER_EVENT_H
#define TIMER_EVENT_H

#include <Arduino.h>
#include <avr/interrupt.h>

// Control constants
#define RUN 1
#define STOP 0

// Maximum number of concurrent timer events
#define MAX_TIMER_EVENTS 4

class TimerEvent {
private:
    void (*userFunction)();
    unsigned long period_us;
    bool enabled;
    unsigned long lastTriggerTime;
    static uint8_t activeTimers;
    static TimerEvent* timerInstances[MAX_TIMER_EVENTS];
    
    // Timer configuration for ATmega328P
    static void setupTimer1();
    static void timerISR();
    
public:
    // Constructor
    TimerEvent(void (*func)(), unsigned long period_microseconds);
    
    // Control operators
    void operator()(int command);
    
    // Getters
    bool isEnabled() const;
    unsigned long getPeriod() const;
    
    // Static methods for timer management
    static void initialize();
    static void update();
};

#endif // TIMER_EVENT_H
