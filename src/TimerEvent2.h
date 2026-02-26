#ifndef TIMER_EVENT2_H
#define TIMER_EVENT2_H

#include <Arduino.h>
#include <avr/interrupt.h>

// Control constants
#define RUN 1
#define STOP 0

// Maximum number of concurrent timer events
#define MAX_TIMER_EVENTS 4

class TimerEvent2 {
private:
    void (*userFunction)();
    unsigned long period_us;
    bool enabled;
    unsigned long lastTriggerTime;
    static uint8_t activeTimers;
    static TimerEvent2* timerInstances[MAX_TIMER_EVENTS];
    
    // Timer configuration for ATmega328P using Timer2
    static void setupTimer2();
    static void timerISR();
    
    // Overflow detection
    static bool hasOverflowed(unsigned long current, unsigned long previous);
    
public:
    // ISR-safe timing using timer counter instead of micros()
    static volatile uint32_t tickCount;
    
    // Constructor
    TimerEvent2(void (*func)(), unsigned long period_microseconds);
    
    // Control operators
    void operator()(int command);
    
    // Getters
    bool isEnabled() const;
    unsigned long getPeriod() const;
    
    // Static methods for timer management
    static void initialize();
    static void update();
    
    // Cleanup
    ~TimerEvent2();
    static void cleanup();
};

#endif // TIMER_EVENT2_H
