#include "TimerEvent.h"
#include <Arduino.h>
#include "power_sensor.h"
#include "pinout.h"
#include "fsm.h"
#include "constants.h"
#include "engine_control.h"

#include "CapacitorCharger.h"
#include <avr/sleep.h>
#include "TimerEvent.h"
#include "led_controller.h"


RGBLED_Controller statusLED(RGB_R_PIN, RGB_G_PIN, RGB_B_PIN); // RGB LED on pins 9, 10, 11

void ledCallback() {
    statusLED.callback();
}

CapacitorCharger capacitor(CAP_SENSOR_PIN, CAP_PWM_PIN, CUR_SENSOR_I2C_ADDR);
TimerEvent ledTimer(ledCallback, 100); // 1 second timer for LED updates


State current_state = State::CHARGING;
MotorController motor(MOTOR_PWM_PIN);


void setup() {
    Serial.begin(9600);
    
    if (!capacitor.begin()) {
        Serial.println("ERROR: Capacitor charger not found!");
        while (1); 
    }
    
    motor.begin();
    statusLED.begin();
    pinMode(LVR_PIN, INPUT);
    ledTimer(RUN);
    
    // CSV header for plotting
    Serial.println("State,Voltage,U_Wake,U_Eco,U_Survival");
}


void loop()
{
    static unsigned long last_print = 0;
    const unsigned long PRINT_INTERVAL = 500; // Print every 500ms

    float capVoltage = capacitor.getVoltage();
    int status = capacitor.charge(U_Cap_Max, 0.750, 4.8);

    // Print data for plotting
    if (millis() - last_print >= PRINT_INTERVAL) {
        Serial.print(static_cast<int>(current_state));
        Serial.print(",");
        Serial.print(capVoltage, 2);
        Serial.print(",");
        Serial.print(U_Wake, 2);
        Serial.print(",");
        Serial.print(U_Eco, 2);
        Serial.print(",");
        Serial.println(U_Survival, 2);
        last_print = millis();
    }

    switch (current_state) {
        case State::CHARGING:
            if (capVoltage > U_Wake) {
                current_state = State::WAKEUP;
            }
            statusLED.setSteady(255, 0, 0);
            break;
        case State::WAKEUP:
            if (status == 1) {
                current_state = State::READY;
            }
            if (digitalRead(LVR_PIN) == LOW) {
                current_state = State::TURBO;
            }
            statusLED.setSteady(255, 165, 0);
            break;
        case State::READY:
            if (digitalRead(LVR_PIN) == LOW) {
                current_state = State::TURBO;
            }
            statusLED.setSteady(0, 255, 0);
            break;
        case State::TURBO:
            if (capVoltage < U_Turbo) {
                current_state = State::ECO;
            }
            motor.eco_power();
            statusLED.setAlternating(0, 0, 255, 255, 0, 0, 200);
            break;
        case State::ECO:
            if (capVoltage < U_Eco) {
                current_state = State::SURVIVAL;
            }
            motor.full_power();
            statusLED.setBlinking(255, 255, 0, 200);
            break;
        case State::SURVIVAL:
            if (capVoltage < U_Survival) {
                current_state = State::SLEEP;
            }
            motor.full_power();
            statusLED.setSteady(255, 0, 0);
            break;
        case State::SLEEP:
            motor.stop();
            set_sleep_mode(SLEEP_MODE_PWR_DOWN);
            sleep_enable();
            sleep_cpu();
            break;
    }
    delay(100); // Main loop delay to reduce CPU load
}