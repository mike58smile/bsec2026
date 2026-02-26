#include "TimerEvent.h"
#include <Arduino.h>
#include "power_sensor.h"
#include "pinout.h"
#include "fsm.h"
#include "constants.h"
#include "button.h"

#include "CapacitorCharger.h"
#include <avr/sleep.h>

CapacitorCharger capacitor;
Button button(BTN_PIN);

State current_state = State::CHARGING;


void setup() {
    // Initialize serial communication
    Serial.begin(9600);
    if (!capacitor.begin()) {
        Serial.println("ERROR: Capacitor charger not found!");
    while (1); 
    }

    button.begin();
}


void loop()
{
    button.update();

    float capVoltage = capacitor.getVoltage();
    switch (current_state) {
        int status;
        case State::CHARGING:
            status = capacitor.charge(U_Cap_Max, 0.5, 4.8);
            if (capVoltage > U_Wake) {
                current_state = State::WAKEUP;
            }
            break;
        case State::WAKEUP:
            status = capacitor.charge(U_Cap_Max, 0.5, 4.8);
            if (status == 1) {
                current_state = State::READY;
            }
            if (button.was_pressed()) {
                current_state = State::TURBO;
            }
            break;
        case State::READY:
            capacitor.stop();
            if (button.was_pressed()) {
                current_state = State::TURBO;
            }
            break;
        case State::TURBO:
            if (capVoltage < U_Eco) {
                current_state = State::ECO;
            }
            break;
        case State::ECO:
            if (capVoltage < U_Survival) {
                current_state = State::SURVIVAL;
            }
            Serial.println("State: ECO");
            break;
        case State::SURVIVAL:
            if (capVoltage < U_Wake) {
                current_state = State::SLEEP;
            }
            Serial.println("State: SURVIVAL");
            break;
        case State::SLEEP:
            set_sleep_mode(SLEEP_MODE_PWR_DOWN);
            sleep_enable();
            sleep_cpu();
            Serial.println("State: SLEEP");
            break;
    }

}