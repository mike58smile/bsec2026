#include "TimerEvent.h"

// Static member initialization
uint8_t TimerEvent::activeTimers = 0;
TimerEvent* TimerEvent::timerInstances[MAX_TIMER_EVENTS] = {nullptr};
volatile uint32_t TimerEvent::tickCount = 0;

TimerEvent::TimerEvent(void (*func)(), unsigned long period_microseconds) 
    : userFunction(func), period_us(period_microseconds), enabled(false), lastTriggerTime(0) {
    
    // Input validation
    if (period_microseconds < 1000) {
        period_us = 1000; // Minimum 1ms
    }
    if (!userFunction) {
        return; // Don't register invalid timers
    }
    
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
    // Use tick count instead of micros() for ISR safety
    uint32_t currentTicks = tickCount;
    
    for (uint8_t i = 0; i < activeTimers; i++) {
        TimerEvent* timer = timerInstances[i];
        if (timer && timer->enabled && timer->userFunction) {
            // Convert microseconds to ticks (1 tick = 1ms)
            uint32_t periodTicks = timer->period_us / 1000;
            
            // Handle overflow safely
            if (hasOverflowed(currentTicks, timer->lastTriggerTime)) {
                // Overflow occurred, reset base time
                timer->lastTriggerTime = currentTicks;
            } else if ((currentTicks - timer->lastTriggerTime) >= periodTicks) {
                timer->lastTriggerTime = currentTicks;
                timer->userFunction();
            }
        }
    }
}

// Timer1 Compare A Match Interrupt Service Routine
ISR(TIMER1_COMPA_vect) {
    // Increment tick counter
    TimerEvent::tickCount++;
    
    // Update all timers directly (now ISR-safe)
    TimerEvent::update();
}

bool TimerEvent::hasOverflowed(unsigned long current, unsigned long previous) {
    return (current < previous) && ((previous - current) > (0xFFFFFFFFUL / 2));
}

TimerEvent::~TimerEvent() {
    // Remove this timer from the active list
    for (uint8_t i = 0; i < activeTimers; i++) {
        if (timerInstances[i] == this) {
            // Shift remaining timers down
            for (uint8_t j = i; j < activeTimers - 1; j++) {
                timerInstances[j] = timerInstances[j + 1];
            }
            timerInstances[activeTimers - 1] = nullptr;
            activeTimers--;
            break;
        }
    }
}

void TimerEvent::cleanup() {
    // Clear all timer instances
    for (uint8_t i = 0; i < MAX_TIMER_EVENTS; i++) {
        timerInstances[i] = nullptr;
    }
    activeTimers = 0;
}
