#include "TimerEvent.h"

// Static member initialization
uint8_t TimerEvent::activeTimers = 0;
TimerEvent* TimerEvent::timerInstances[MAX_TIMER_EVENTS] = {nullptr};

TimerEvent::TimerEvent(void (*func)(), unsigned long period_microseconds) 
    : userFunction(func), period_us(period_microseconds), enabled(false), lastTriggerTime(0) {
    
    // Register this timer instance
    if (activeTimers < MAX_TIMER_EVENTS) {
        timerInstances[activeTimers] = this;
        activeTimers++;
        
        // Initialize timer system on first timer creation
        if (activeTimers == 1) {
            initialize();
        }
    }
}

void TimerEvent::operator()(int command) {
    if (command == RUN) {
        enabled = true;
        lastTriggerTime = micros();
    } else if (command == STOP) {
        enabled = false;
    }
}

bool TimerEvent::isEnabled() const {
    return enabled;
}

unsigned long TimerEvent::getPeriod() const {
    return period_us;
}

void TimerEvent::initialize() {
    // Configure Timer1 for CTC mode with 1ms interrupts
    // Using prescaler 64: 16MHz / 64 = 250kHz
    // CTC with OCR1A = 249 gives 250kHz / 250 = 1kHz = 1ms
    noInterrupts();
    
    TCCR1A = 0;  // Normal port operation, OC1A/OC1B disconnected
    TCCR1B = 0;  // Stop timer
    
    // Set CTC mode
    TCCR1B |= (1 << WGM12);
    
    // Set prescaler to 64
    TCCR1B |= (1 << CS11) | (1 << CS10);
    
    // Set compare value for 1ms interrupt
    OCR1A = 249;
    
    // Enable compare A match interrupt
    TIMSK1 |= (1 << OCIE1A);
    
    TCNT1 = 0;  // Reset counter
    
    interrupts();
}

void TimerEvent::update() {
    unsigned long currentTime = micros();
    
    for (uint8_t i = 0; i < activeTimers; i++) {
        TimerEvent* timer = timerInstances[i];
        if (timer && timer->enabled && timer->userFunction) {
            if ((currentTime - timer->lastTriggerTime) >= timer->period_us) {
                timer->lastTriggerTime = currentTime;
                timer->userFunction();
            }
        }
    }
}

// Timer1 Compare A Match Interrupt Service Routine
ISR(TIMER1_COMPA_vect) {
    static uint16_t millisecondCounter = 0;
    millisecondCounter++;
    
    // Call update every millisecond
    TimerEvent::update();
}
