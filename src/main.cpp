#include <Arduino.h>
#include "pinout.h"
#include "fsm.h"
#include "constants.h"
#include "engine_control.h"

#include "CapacitorCharger.h"
#include <avr/sleep.h>
#include "led_controller.h"

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>


RGBLED_Controller statusLED(RGB_R_PIN, RGB_G_PIN, RGB_B_PIN); // RGB LED on pins 3, 5, 6

CapacitorCharger capacitor(CAP_SENSOR_PIN, CAP_PWM_PIN, CUR_SENSOR_I2C_ADDR);


State current_state = State::CHARGING;
MotorController motor(MOTOR_PWM_PIN);

// OLED Display
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &SPI, DISPLAY_DC_PIN, DISPLAY_RESET_PIN, -1, 1000000UL);

// Face geometry constants
#define FACE_CX  64
#define FACE_CY  30
#define EYE_LX   48
#define EYE_RX   80
#define EYE_Y    26
#define MOUTH_Y  42

#define FRAME_MS 80
int stateFrame = 0;
unsigned long lastFrameTime = 0;
State previousState = State::CHARGING;

// --- Helper: draw rounded face outline ---
void drawFaceOutline() {
  display.drawRoundRect(14, 4, 100, 52, 12, SSD1306_WHITE);
}

// --- Helper: draw status bar text at bottom ---
void drawStatusBar(const __FlashStringHelper* label) {
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 57);
  display.print(label);
}

// --- Helper: draw a cute blush (two small circles on cheeks) ---
void drawBlush() {
  display.fillCircle(36, 38, 3, SSD1306_WHITE);
  display.fillCircle(92, 38, 3, SSD1306_WHITE);
}

// --- Helper: draw battery icon at right side of status bar ---
void drawBattery(int level) {
  int bx = 108, by = 56;
  display.drawRect(bx, by, 18, 8, SSD1306_WHITE);
  display.fillRect(bx + 18, by + 2, 2, 4, SSD1306_WHITE);
  for (int i = 0; i < level; i++) {
    display.fillRect(bx + 2 + i * 4, by + 2, 3, 4, SSD1306_WHITE);
  }
}

// ======================== CHARGING ========================
void animCharging(int frame) {
  display.clearDisplay();
  drawFaceOutline();

  int blink = (frame % 20 == 0);
  if (blink) {
    display.drawLine(EYE_LX - 6, EYE_Y, EYE_LX + 6, EYE_Y, SSD1306_WHITE);
    display.drawLine(EYE_RX - 6, EYE_Y, EYE_RX + 6, EYE_Y, SSD1306_WHITE);
  } else {
    display.drawLine(EYE_LX - 5, EYE_Y + 2, EYE_LX, EYE_Y - 4, SSD1306_WHITE);
    display.drawLine(EYE_LX, EYE_Y - 4, EYE_LX + 5, EYE_Y + 2, SSD1306_WHITE);
    display.drawLine(EYE_RX - 5, EYE_Y + 2, EYE_RX, EYE_Y - 4, SSD1306_WHITE);
    display.drawLine(EYE_RX, EYE_Y - 4, EYE_RX + 5, EYE_Y + 2, SSD1306_WHITE);
  }

  display.drawLine(FACE_CX - 8, MOUTH_Y, FACE_CX - 3, MOUTH_Y + 4, SSD1306_WHITE);
  display.drawLine(FACE_CX - 3, MOUTH_Y + 4, FACE_CX + 3, MOUTH_Y + 4, SSD1306_WHITE);
  display.drawLine(FACE_CX + 3, MOUTH_Y + 4, FACE_CX + 8, MOUTH_Y, SSD1306_WHITE);

  drawBlush();

  if ((frame / 4) % 2 == 0) {
    display.fillTriangle(FACE_CX - 2, EYE_Y - 10, FACE_CX + 4, EYE_Y - 4, FACE_CX, EYE_Y - 4, SSD1306_WHITE);
    display.fillTriangle(FACE_CX, EYE_Y - 4, FACE_CX + 2, EYE_Y - 4, FACE_CX - 2, EYE_Y + 2, SSD1306_WHITE);
  }

  drawStatusBar(F("CHARGING"));
  drawBattery((frame / 8) % 5);
  display.display();
}

// ======================== WAKEUP ========================
void animWakeup(int frame) {
  display.clearDisplay();
  drawFaceOutline();

  int openness = min(frame, 8);

  if (openness < 3) {
    display.drawLine(EYE_LX - 6, EYE_Y, EYE_LX + 6, EYE_Y, SSD1306_WHITE);
    display.drawLine(EYE_RX - 6, EYE_Y, EYE_RX + 6, EYE_Y, SSD1306_WHITE);
  } else if (openness < 6) {
    display.drawRoundRect(EYE_LX - 5, EYE_Y - 2, 10, 5, 2, SSD1306_WHITE);
    display.drawRoundRect(EYE_RX - 5, EYE_Y - 2, 10, 5, 2, SSD1306_WHITE);
    display.fillCircle(EYE_LX, EYE_Y, 1, SSD1306_WHITE);
    display.fillCircle(EYE_RX, EYE_Y, 1, SSD1306_WHITE);
  } else {
    display.drawCircle(EYE_LX, EYE_Y, 7, SSD1306_WHITE);
    display.drawCircle(EYE_RX, EYE_Y, 7, SSD1306_WHITE);
    display.fillCircle(EYE_LX, EYE_Y, 3, SSD1306_WHITE);
    display.fillCircle(EYE_RX, EYE_Y, 3, SSD1306_WHITE);
    display.drawPixel(EYE_LX + 2, EYE_Y - 2, SSD1306_BLACK);
    display.drawPixel(EYE_RX + 2, EYE_Y - 2, SSD1306_BLACK);
  }

  if (openness < 5) {
    display.drawCircle(FACE_CX, MOUTH_Y + 2, 3 + openness / 2, SSD1306_WHITE);
  } else {
    display.drawLine(FACE_CX - 6, MOUTH_Y, FACE_CX - 2, MOUTH_Y + 3, SSD1306_WHITE);
    display.drawLine(FACE_CX - 2, MOUTH_Y + 3, FACE_CX + 2, MOUTH_Y + 3, SSD1306_WHITE);
    display.drawLine(FACE_CX + 2, MOUTH_Y + 3, FACE_CX + 6, MOUTH_Y, SSD1306_WHITE);
  }

  if (frame < 10) {
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(95, 8);
    if (frame < 3) display.print(F("zzz"));
    else if (frame < 6) display.print(F("zz"));
    else display.print(F("z"));
  }

  drawStatusBar(F("WAKING UP"));
  display.display();
}

// ======================== READY ========================
void animReady(int frame) {
  display.clearDisplay();
  drawFaceOutline();

  int blink = (frame % 30 == 0);

  if (blink) {
    display.drawLine(EYE_LX - 6, EYE_Y, EYE_LX + 6, EYE_Y, SSD1306_WHITE);
    display.drawLine(EYE_RX - 6, EYE_Y, EYE_RX + 6, EYE_Y, SSD1306_WHITE);
  } else {
    display.drawLine(EYE_LX - 6, EYE_Y - 4, EYE_LX + 6, EYE_Y - 4, SSD1306_WHITE);
    display.drawCircle(EYE_LX, EYE_Y, 6, SSD1306_WHITE);
    display.fillCircle(EYE_LX, EYE_Y, 2, SSD1306_WHITE);
    display.drawLine(EYE_RX - 6, EYE_Y - 4, EYE_RX + 6, EYE_Y - 4, SSD1306_WHITE);
    display.drawCircle(EYE_RX, EYE_Y, 6, SSD1306_WHITE);
    display.fillCircle(EYE_RX, EYE_Y, 2, SSD1306_WHITE);
  }

  display.drawLine(FACE_CX - 10, MOUTH_Y, FACE_CX - 4, MOUTH_Y + 5, SSD1306_WHITE);
  display.drawLine(FACE_CX - 4, MOUTH_Y + 5, FACE_CX + 4, MOUTH_Y + 5, SSD1306_WHITE);
  display.drawLine(FACE_CX + 4, MOUTH_Y + 5, FACE_CX + 10, MOUTH_Y, SSD1306_WHITE);

  if ((frame / 6) % 2 == 0) {
    display.drawPixel(100, 10, SSD1306_WHITE);
    display.drawLine(98, 10, 102, 10, SSD1306_WHITE);
    display.drawLine(100, 8, 100, 12, SSD1306_WHITE);
  }

  drawStatusBar(F("READY"));
  drawBattery(4);
  display.display();
}

// ======================== TURBO ========================
void animTurbo(int frame) {
  display.clearDisplay();
  drawFaceOutline();

  display.drawCircle(EYE_LX, EYE_Y, 8, SSD1306_WHITE);
  display.drawCircle(EYE_RX, EYE_Y, 8, SSD1306_WHITE);
  display.fillCircle(EYE_LX, EYE_Y, 4, SSD1306_WHITE);
  display.fillCircle(EYE_RX, EYE_Y, 4, SSD1306_WHITE);
  display.drawPixel(EYE_LX + 3, EYE_Y - 3, SSD1306_BLACK);
  display.drawPixel(EYE_RX + 3, EYE_Y - 3, SSD1306_BLACK);

  display.drawLine(FACE_CX - 14, MOUTH_Y - 1, FACE_CX - 6, MOUTH_Y + 6, SSD1306_WHITE);
  display.drawLine(FACE_CX - 6, MOUTH_Y + 6, FACE_CX + 6, MOUTH_Y + 6, SSD1306_WHITE);
  display.drawLine(FACE_CX + 6, MOUTH_Y + 6, FACE_CX + 14, MOUTH_Y - 1, SSD1306_WHITE);
  display.drawLine(FACE_CX - 14, MOUTH_Y - 1, FACE_CX + 14, MOUTH_Y - 1, SSD1306_WHITE);

  int offset = frame % 6;
  for (int i = 0; i < 4; i++) {
    int y = 12 + i * 12 + offset;
    display.drawLine(0, y, 10, y, SSD1306_WHITE);
  }

  if ((frame / 2) % 2 == 0) {
    display.fillTriangle(118, 25, 127, 30, 118, 35, SSD1306_WHITE);
  } else {
    display.fillTriangle(120, 27, 127, 30, 120, 33, SSD1306_WHITE);
  }

  drawBlush();
  drawStatusBar(F("TURBO!"));
  drawBattery(4);
  display.display();
}

// ======================== ECO ========================
void animEco(int frame) {
  display.clearDisplay();
  drawFaceOutline();

  int blink = (frame % 25 == 0);

  if (blink) {
    display.drawLine(EYE_LX - 6, EYE_Y, EYE_LX + 6, EYE_Y, SSD1306_WHITE);
    display.drawLine(EYE_RX - 6, EYE_Y, EYE_RX + 6, EYE_Y, SSD1306_WHITE);
  } else {
    display.drawLine(EYE_LX - 5, EYE_Y - 2, EYE_LX, EYE_Y + 2, SSD1306_WHITE);
    display.drawLine(EYE_LX, EYE_Y + 2, EYE_LX + 5, EYE_Y - 2, SSD1306_WHITE);
    display.drawLine(EYE_RX - 5, EYE_Y - 2, EYE_RX, EYE_Y + 2, SSD1306_WHITE);
    display.drawLine(EYE_RX, EYE_Y + 2, EYE_RX + 5, EYE_Y - 2, SSD1306_WHITE);
  }

  display.drawLine(FACE_CX - 6, MOUTH_Y, FACE_CX - 2, MOUTH_Y + 3, SSD1306_WHITE);
  display.drawLine(FACE_CX - 2, MOUTH_Y + 3, FACE_CX + 2, MOUTH_Y + 3, SSD1306_WHITE);
  display.drawLine(FACE_CX + 2, MOUTH_Y + 3, FACE_CX + 6, MOUTH_Y, SSD1306_WHITE);

  int leafX = 100 + (int)(3.0 * sin(frame * 0.3));
  int leafY = 8 + (frame % 12);
  display.drawCircle(leafX, leafY, 3, SSD1306_WHITE);
  display.drawLine(leafX, leafY + 3, leafX, leafY + 6, SSD1306_WHITE);

  drawBlush();
  drawStatusBar(F("ECO"));
  drawBattery(2);
  display.display();
}

// ======================== SURVIVAL ========================
void animSurvival(int frame) {
  display.clearDisplay();
  drawFaceOutline();

  display.drawCircle(EYE_LX, EYE_Y, 6, SSD1306_WHITE);
  display.drawCircle(EYE_RX, EYE_Y, 6, SSD1306_WHITE);
  display.fillCircle(EYE_LX, EYE_Y + 1, 2, SSD1306_WHITE);
  display.fillCircle(EYE_RX, EYE_Y + 1, 2, SSD1306_WHITE);
  display.drawLine(EYE_LX - 7, EYE_Y - 10, EYE_LX + 3, EYE_Y - 8, SSD1306_WHITE);
  display.drawLine(EYE_RX - 3, EYE_Y - 8, EYE_RX + 7, EYE_Y - 10, SSD1306_WHITE);

  int wobble = (frame % 4 < 2) ? 0 : 1;
  display.drawLine(FACE_CX - 8, MOUTH_Y + wobble, FACE_CX - 3, MOUTH_Y + 2 + wobble, SSD1306_WHITE);
  display.drawLine(FACE_CX - 3, MOUTH_Y + 2 + wobble, FACE_CX + 3, MOUTH_Y + wobble, SSD1306_WHITE);
  display.drawLine(FACE_CX + 3, MOUTH_Y + wobble, FACE_CX + 8, MOUTH_Y + 2 + wobble, SSD1306_WHITE);

  int sweatY = 10 + (frame % 10);
  display.fillCircle(95, sweatY + 3, 2, SSD1306_WHITE);
  display.fillTriangle(93, sweatY + 2, 97, sweatY + 2, 95, sweatY - 2, SSD1306_WHITE);

  drawStatusBar(F("SURVIVAL"));
  drawBattery(1);
  display.display();
}

// ======================== SLEEP ========================
void animSleep(int frame) {
  display.clearDisplay();
  drawFaceOutline();

  display.drawLine(EYE_LX - 5, EYE_Y, EYE_LX, EYE_Y + 2, SSD1306_WHITE);
  display.drawLine(EYE_LX, EYE_Y + 2, EYE_LX + 5, EYE_Y, SSD1306_WHITE);
  display.drawLine(EYE_RX - 5, EYE_Y, EYE_RX, EYE_Y + 2, SSD1306_WHITE);
  display.drawLine(EYE_RX, EYE_Y + 2, EYE_RX + 5, EYE_Y, SSD1306_WHITE);

  display.drawLine(FACE_CX - 3, MOUTH_Y + 2, FACE_CX + 3, MOUTH_Y + 2, SSD1306_WHITE);

  drawBlush();

  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  int zOffset = (frame / 3) % 8;
  display.setCursor(90, 14 - zOffset);
  display.print('z');
  display.setCursor(97, 8 - zOffset);
  display.setTextSize(1);
  display.print('z');
  display.setCursor(104, 2 - min(zOffset, 4));
  display.print('Z');

  drawStatusBar(F("SLEEP"));
  drawBattery(0);
  display.display();
}

void stabilizeDisplayController() {
  display.stopscroll();
  display.ssd1306_command(SSD1306_NORMALDISPLAY);
  display.ssd1306_command(SSD1306_DISPLAYALLON_RESUME);
  display.ssd1306_command(SSD1306_SEGREMAP | 0x1);
  display.ssd1306_command(SSD1306_COMSCANDEC);
  display.ssd1306_command(SSD1306_SETDISPLAYOFFSET);
  display.ssd1306_command(0x00);
  display.ssd1306_command(SSD1306_SETSTARTLINE | 0x00);
}


void setup() {
    Serial.begin(9600);

    // OLED Display initialization
    delay(500);
    if(!display.begin(SSD1306_SWITCHCAPVCC)) {
        Serial.println(F("SSD1306 allocation failed"));
        for(;;);
    }
    display.stopscroll();
    display.ssd1306_command(SSD1306_NORMALDISPLAY);
    display.ssd1306_command(SSD1306_DISPLAYALLON_RESUME);
    display.ssd1306_command(SSD1306_SEGREMAP | 0x1);
    display.ssd1306_command(SSD1306_COMSCANDEC);
    display.ssd1306_command(SSD1306_SETDISPLAYOFFSET);
    display.ssd1306_command(0x00);
    display.ssd1306_command(SSD1306_SETSTARTLINE | 0x00);
    display.setTextWrap(false);
    display.clearDisplay();
    display.display();
    delay(500);

    if (!capacitor.begin()) {
        Serial.println("ERROR: Capacitor charger not found!");
        while (1); 
    }
    
    motor.begin();
    statusLED.begin();
    pinMode(LVR_PIN, INPUT);
    
    // CSV header for plotting
    Serial.println("State,Voltage,U_Wake,U_Eco,U_Survival");
}


void loop()
{
    static unsigned long last_print = 0;
    const unsigned long PRINT_INTERVAL = 500; // Print every 500ms
    static unsigned long last_flash = 0;
    const unsigned long FLASH_INTERVAL = 200; // Flash every 200ms
    static unsigned long last_eco_flash = 0;
    const unsigned long ECO_FLASH_INTERVAL = 500; // Eco flash every 500ms
    static unsigned long last_breath = 0;
    const unsigned long BREATH_INTERVAL = 50; // Breath update every 50ms
    static bool flash_state = false;
    static bool eco_flash_state = false;
    static bool was_in_turbo = false;
    static int breath_value = 0;
    static bool breathing_up = true;

    float capVoltage = capacitor.getVoltage();
    int status = capacitor.charge(U_Cap_Max, 0.750, 4.8);

    // OLED frame timing
    unsigned long now = millis();
    bool newFrame = (now - lastFrameTime >= FRAME_MS);
    if (newFrame) {
        lastFrameTime = now;
        if ((stateFrame % 12) == 0) {
            stabilizeDisplayController();
        }
    }

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
            motor.stop();
            statusLED.set(255, 0, 0);
            if (newFrame) animCharging(stateFrame);
            break;
        case State::WAKEUP:
            if (status == 1) {
                current_state = State::READY;
            }
            if (digitalRead(LVR_PIN) == LOW) {
                current_state = State::TURBO;
            }
            statusLED.set(255, 165, 0);
            if (newFrame) animWakeup(stateFrame);
            break;
        case State::READY:
            if (digitalRead(LVR_PIN) == LOW) {
                current_state = State::TURBO;
            }
            statusLED.set(0, 255, 0);
            if (newFrame) animReady(stateFrame);
            break;
        case State::TURBO:
            if (capVoltage < U_Turbo) {
                current_state = State::ECO;
            }
            motor.eco_power();
            
            // Police car flashing: red/blue alternating every 200ms
            if (!was_in_turbo) {
                last_flash = millis();
                flash_state = false;
                was_in_turbo = true;
            }
            
            if (millis() - last_flash >= FLASH_INTERVAL) {
                flash_state = !flash_state;
                last_flash = millis();
            }
            
            if (flash_state) {
                statusLED.set(255, 0, 0); // Red
            } else {
                statusLED.set(0, 0, 255); // Blue
            }
            if (newFrame) animTurbo(stateFrame);
            break;
        case State::ECO:
            if (capVoltage < U_Eco) {
                current_state = State::SURVIVAL;
            }
            motor.full_power();
            
            // Reset turbo mode flag when leaving turbo
            was_in_turbo = false;
            
            // Yellow flashing every 500ms
            if (millis() - last_eco_flash >= ECO_FLASH_INTERVAL) {
                eco_flash_state = !eco_flash_state;
                last_eco_flash = millis();
            }
            
            if (eco_flash_state) {
                statusLED.set(255, 255, 0); // Yellow
            } else {
                statusLED.off(); // Off
            }
            if (newFrame) animEco(stateFrame);
            break;
        case State::SURVIVAL:
            if (capVoltage < U_Survival) {
                current_state = State::SLEEP;
            }
            motor.full_power();
            
            // Reset turbo mode flag when leaving turbo
            was_in_turbo = false;
            
            // Breathing red LED effect
            if (millis() - last_breath >= BREATH_INTERVAL) {
                last_breath = millis();
                
                if (breathing_up) {
                    breath_value += 20;
                    if (breath_value >= 125) {
                        breath_value = 125;
                        breathing_up = false;
                    }
                } else {
                    breath_value -= 20;
                    if (breath_value <= 10) {
                        breath_value = 10;
                        breathing_up = true;
                    }
                }
                
                statusLED.set(breath_value, 0, 0); // Breathing red
            }
            if (newFrame) animSurvival(stateFrame);
            break;
        case State::SLEEP:
            motor.stop();
            statusLED.off();
            if (capVoltage > U_Wake) {
                current_state = State::WAKEUP;
            }
            set_sleep_mode(SLEEP_MODE_PWR_DOWN);
            sleep_enable();
            sleep_cpu();
            if (newFrame) animSleep(stateFrame);
            break;
    }

    // OLED frame counter management
    if (newFrame) {
        if (current_state != previousState) {
            stateFrame = 0;
            previousState = current_state;
        }
        stateFrame++;
    }

    delay(100); // Main loop delay to reduce CPU load
}