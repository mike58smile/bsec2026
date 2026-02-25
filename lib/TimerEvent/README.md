# TimerEvent Library for Arduino Uno

A simple and efficient timer library for Arduino Uno (ATmega328P) that enables periodic function execution with microsecond precision.

## Features

- **Easy to use**: Define timers with `TimerEvent(my_func, period_us)`
- **Simple control**: Enable with `timer(RUN)` and disable with `timer(STOP)`
- **Microsecond precision**: Specify periods in microseconds
- **Multiple timers**: Support for up to 4 concurrent timer events
- **Hardware-based**: Uses ATmega328P Timer1 for efficient operation
- **Non-blocking**: Uses interrupts, doesn't block main loop

## Usage

```cpp
#include <Arduino.h>
#include "TimerEvent.h"

// Define your periodic function
void myFunction() {
    Serial.println("Hello every second!");
}

// Create timer with 1 second period (1,000,000 microseconds)
TimerEvent myTimer(myFunction, 1000000);

void setup() {
    Serial.begin(9600);
    
    // Start the timer
    myTimer(RUN);
}

void loop() {
    // Your main code here
    // Timer runs automatically in background via interrupts
}
```

## API Reference

### Constructor

```cpp
TimerEvent(void (*func)(), unsigned long period_microseconds)
```

- `func`: Function pointer to be called periodically
- `period_microseconds`: Period in microseconds (min 1000 for 1ms)

### Control

```cpp
timer(RUN);   // Start the timer
timer(STOP);  // Stop the timer
```

### Status

```cpp
bool enabled = timer.isEnabled();           // Check if timer is running
unsigned long period = timer.getPeriod();   // Get period in microseconds
```

## Technical Details

- Uses Timer1 in CTC mode with 1ms interrupt resolution
- Supports up to 4 concurrent timer events
- Interrupt-driven for non-blocking operation
- Compatible with Arduino Uno (ATmega328P)

## Installation

1. Copy the `TimerEvent` folder to your Arduino `libraries` directory
2. Or use with PlatformIO as shown in the example

## Example

See `lib/TimerEvent/showcase.cpp` for a complete example demonstrating multiple timers with different periods.
