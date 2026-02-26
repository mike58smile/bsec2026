#include "TimerEvent.h"
#include <Arduino.h>
#include "pinout.h"
#include "fsm.h"
#include "constants.h"
#include "engine_control.h"

#include "CapacitorCharger.h"
#include <avr/sleep.h>
#include "TimerEvent.h"
#include "led_controller.h"


RGBLED_Controller statusLED(RGB_R_PIN, RGB_G_PIN, RGB_B_PIN); // RGB LED on pins 9, 10, 11


CapacitorCharger capacitor(CAP_SENSOR_PIN, CAP_PWM_PIN, CUR_SENSOR_I2C_ADDR);


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
    
    // CSV header for plotting
    Serial.println("Status: State, CapVoltage(V), PanelVoltage(V), Current(mA), Power(mW), U_Wake(V), U_Eco(V), U_Survival(V)");
}


void loop()
{
    static unsigned long last_print = 0;
    const unsigned long PRINT_INTERVAL = 500; // Print every 500ms

    static float capVoltage = 0;
    int status = capacitor.charge(U_Cap_Max, 0.75, 4.8);

    // Print data for plotting
    if (millis() - last_print >= PRINT_INTERVAL) {
        capVoltage = capacitor.getVoltage(); // Only measure when printing
        Serial.print("State:");
        Serial.print(static_cast<int>(current_state));
        Serial.print(", CapVoltage:");
        Serial.print(capVoltage, 2);
        Serial.print("V, PanelVoltage:");
        Serial.print(capacitor.getPanelVoltage(), 2);
        Serial.print("V, Current:");
        Serial.print(capacitor.getCurrent(), 0);
        Serial.print("mA, Power:");
        Serial.print(capacitor.getPanelVoltage() * capacitor.getCurrent(), 0);
        Serial.print("mW, U_Wake:");
        Serial.print(U_Wake, 2);
        Serial.print("V, U_Eco:");
        Serial.print(U_Eco, 2);
        Serial.print("V, U_Survival:");
        Serial.print(U_Survival, 2);
        Serial.println("V");
        last_print = millis();
    }

    switch (current_state) {
        case State::CHARGING:
            if (capVoltage > U_Wake) {
                current_state = State::WAKEUP;
            }
            motor.stop();
            statusLED.set(255, 0, 0);
            break;
        case State::WAKEUP:
            if (status == 1) {
                current_state = State::READY;
            }
            if (digitalRead(LVR_PIN) == LOW) {
                current_state = State::TURBO;
            }
            statusLED.set(255, 165, 0);
            break;
        case State::READY:
            if (digitalRead(LVR_PIN) == LOW) {
                current_state = State::TURBO;
            }
            statusLED.set(0, 255, 0);
            break;
        case State::TURBO:
            if (capVoltage < U_Turbo) {
                current_state = State::ECO;
            }
            motor.eco_power();
            statusLED.set(0, 0, 255);
            break;
        case State::ECO:
            if (capVoltage < U_Eco) {
                current_state = State::SURVIVAL;
            }
            motor.full_power();
            statusLED.set(255, 255, 0);
            break;
        case State::SURVIVAL:
            if (capVoltage < U_Survival) {
                current_state = State::SLEEP;
            }
            motor.full_power();
            statusLED.set(125, 0, 0);
            break;
        case State::SLEEP:
            motor.stop();
            statusLED.off();
            if (capVoltage > U_Wake) {
                current_state = State::WAKEUP;
            }
            break;
    }
    delay(100); // Main loop delay to reduce CPU load
}