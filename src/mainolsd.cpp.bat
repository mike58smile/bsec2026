#include <Arduino.h>
/**************************************************************************
 This is an example for our Monochrome OLEDs based on SSD1306 drivers

 Pick one up today in the adafruit shop!
 ------> http://www.adafruit.com/category/63_98

 This example is for a 128x64 pixel display using I2C to communicate
 3 pins are required to interface (two I2C and one reset).

 Adafruit invests time and resources providing this open
 source code, please support Adafruit and open-source
 hardware by purchasing products from Adafruit!

 Written by Limor Fried/Ladyada for Adafruit Industries,
 with contributions from the open source community.
 BSD license, check license.txt for more information
 All text above, and the splash screen below must be
 included in any redistribution.
 **************************************************************************/

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for SSD1306 display connected via 4-wire SPI
// Hardware SPI pins on Arduino Uno: MOSI = D11, SCLK = D13
#define OLED_DC     7
#define OLED_CS    -1  // no CS pin
#define OLED_RESET  8
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &SPI, OLED_DC, OLED_RESET, OLED_CS, 1000000UL);

#define NUMFLAKES     10 // Number of snowflakes in the animation example

#define LOGO_HEIGHT   16
#define LOGO_WIDTH    16
static const unsigned char PROGMEM logo_bmp[] =
{ 0b00000000, 0b11000000,
  0b00000001, 0b11000000,
  0b00000001, 0b11000000,
  0b00000011, 0b11100000,
  0b11110011, 0b11100000,
  0b11111110, 0b11111000,
  0b01111110, 0b11111111,
  0b00110011, 0b10011111,
  0b00011111, 0b11111100,
  0b00001101, 0b01110000,
  0b00011011, 0b10100000,
  0b00111111, 0b11100000,
  0b00111111, 0b11110000,
  0b01111100, 0b11110000,
  0b01110000, 0b01110000,
  0b00000000, 0b00110000 };


void testDimensions() {
  display.clearDisplay();

  // 1-pixel border around the entire screen — if any edge is cut off, you'll see it
  display.drawRect(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, SSD1306_WHITE);

  // Corner markers (5x5 filled squares in each corner)
  display.fillRect(0, 0, 5, 5, SSD1306_WHITE);                                      // top-left
  display.fillRect(SCREEN_WIDTH - 5, 0, 5, 5, SSD1306_WHITE);                       // top-right
  display.fillRect(0, SCREEN_HEIGHT - 5, 5, 5, SSD1306_WHITE);                      // bottom-left
  display.fillRect(SCREEN_WIDTH - 5, SCREEN_HEIGHT - 5, 5, 5, SSD1306_WHITE);       // bottom-right

  // Center crosshair
  display.drawLine(SCREEN_WIDTH / 2, 0, SCREEN_WIDTH / 2, SCREEN_HEIGHT - 1, SSD1306_WHITE);  // vertical
  display.drawLine(0, SCREEN_HEIGHT / 2, SCREEN_WIDTH - 1, SCREEN_HEIGHT / 2, SSD1306_WHITE); // horizontal

  // Print dimensions in center
  display.setTextSize(1);
  display.setTextColor(SSD1306_BLACK, SSD1306_WHITE); // inverse text for visibility on crosshair
  display.setCursor(SCREEN_WIDTH / 2 - 20, SCREEN_HEIGHT / 2 - 12);
  display.print(SCREEN_WIDTH);
  display.print('x');
  display.print(SCREEN_HEIGHT);

  // Corner coordinate labels
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(7, 7);
  display.print(F("0,0"));
  display.setCursor(SCREEN_WIDTH - 42, SCREEN_HEIGHT - 15);
  display.print(SCREEN_WIDTH - 1);
  display.print(',');
  display.print(SCREEN_HEIGHT - 1);

  display.display();
  delay(5000);
}

void testdrawline() {
  int16_t i;

  display.clearDisplay(); // Clear display buffer

  for(i=0; i<display.width(); i+=4) {
    display.drawLine(0, 0, i, display.height()-1, SSD1306_WHITE);
    display.display(); // Update screen with each newly-drawn line
    delay(1);
  }
  for(i=0; i<display.height(); i+=4) {
    display.drawLine(0, 0, display.width()-1, i, SSD1306_WHITE);
    display.display();
    delay(1);
  }
  delay(250);

  display.clearDisplay();

  for(i=0; i<display.width(); i+=4) {
    display.drawLine(0, display.height()-1, i, 0, SSD1306_WHITE);
    display.display();
    delay(1);
  }
  for(i=display.height()-1; i>=0; i-=4) {
    display.drawLine(0, display.height()-1, display.width()-1, i, SSD1306_WHITE);
    display.display();
    delay(1);
  }
  delay(250);

  display.clearDisplay();

  for(i=display.width()-1; i>=0; i-=4) {
    display.drawLine(display.width()-1, display.height()-1, i, 0, SSD1306_WHITE);
    display.display();
    delay(1);
  }
  for(i=display.height()-1; i>=0; i-=4) {
    display.drawLine(display.width()-1, display.height()-1, 0, i, SSD1306_WHITE);
    display.display();
    delay(1);
  }
  delay(250);

  display.clearDisplay();

  for(i=0; i<display.height(); i+=4) {
    display.drawLine(display.width()-1, 0, 0, i, SSD1306_WHITE);
    display.display();
    delay(1);
  }
  for(i=0; i<display.width(); i+=4) {
    display.drawLine(display.width()-1, 0, i, display.height()-1, SSD1306_WHITE);
    display.display();
    delay(1);
  }

  delay(2000); // Pause for 2 seconds
}

void testdrawrect(void) {
  display.clearDisplay();

  for(int16_t i=0; i<display.height()/2; i+=2) {
    display.drawRect(i, i, display.width()-2*i, display.height()-2*i, SSD1306_WHITE);
    display.display(); // Update screen with each newly-drawn rectangle
    delay(1);
  }

  delay(2000);
}

void testfillrect(void) {
  display.clearDisplay();

  for(int16_t i=0; i<display.height()/2; i+=3) {
    // The INVERSE color is used so rectangles alternate white/black
    display.fillRect(i, i, display.width()-i*2, display.height()-i*2, SSD1306_INVERSE);
    display.display(); // Update screen with each newly-drawn rectangle
    delay(1);
  }

  delay(2000);
}

void testdrawcircle(void) {
  display.clearDisplay();

  for(int16_t i=0; i<max(display.width(),display.height())/2; i+=2) {
    display.drawCircle(display.width()/2, display.height()/2, i, SSD1306_WHITE);
    display.display();
    delay(1);
  }

  delay(2000);
}

void testfillcircle(void) {
  display.clearDisplay();

  for(int16_t i=max(display.width(),display.height())/2; i>0; i-=3) {
    // The INVERSE color is used so circles alternate white/black
    display.fillCircle(display.width() / 2, display.height() / 2, i, SSD1306_INVERSE);
    display.display(); // Update screen with each newly-drawn circle
    delay(1);
  }

  delay(2000);
}

void testdrawroundrect(void) {
  display.clearDisplay();

  for(int16_t i=0; i<display.height()/2-2; i+=2) {
    display.drawRoundRect(i, i, display.width()-2*i, display.height()-2*i,
      display.height()/4, SSD1306_WHITE);
    display.display();
    delay(1);
  }

  delay(2000);
}

void testfillroundrect(void) {
  display.clearDisplay();

  for(int16_t i=0; i<display.height()/2-2; i+=2) {
    // The INVERSE color is used so round-rects alternate white/black
    display.fillRoundRect(i, i, display.width()-2*i, display.height()-2*i,
      display.height()/4, SSD1306_INVERSE);
    display.display();
    delay(1);
  }

  delay(2000);
}

void testdrawtriangle(void) {
  display.clearDisplay();

  for(int16_t i=0; i<max(display.width(),display.height())/2; i+=5) {
    display.drawTriangle(
      display.width()/2  , display.height()/2-i,
      display.width()/2-i, display.height()/2+i,
      display.width()/2+i, display.height()/2+i, SSD1306_WHITE);
    display.display();
    delay(1);
  }

  delay(2000);
}

void testfilltriangle(void) {
  display.clearDisplay();

  for(int16_t i=max(display.width(),display.height())/2; i>0; i-=5) {
    // The INVERSE color is used so triangles alternate white/black
    display.fillTriangle(
      display.width()/2  , display.height()/2-i,
      display.width()/2-i, display.height()/2+i,
      display.width()/2+i, display.height()/2+i, SSD1306_INVERSE);
    display.display();
    delay(1);
  }

  delay(2000);
}

void testdrawchar(void) {
  display.clearDisplay();

  display.setTextSize(1);      // Normal 1:1 pixel scale
  display.setTextColor(SSD1306_WHITE); // Draw white text
  display.setCursor(0, 0);     // Start at top-left corner
  display.cp437(true);         // Use full 256 char 'Code Page 437' font

  // Not all the characters will fit on the display. This is normal.
  // Library will draw what it can and the rest will be clipped.
  for(int16_t i=0; i<256; i++) {
    if(i == '\n') display.write(' ');
    else          display.write(i);
  }

  display.display();
  delay(2000);
}

void testdrawstyles(void) {
  display.clearDisplay();

  display.setTextSize(1);             // Normal 1:1 pixel scale
  display.setTextColor(SSD1306_WHITE);        // Draw white text
  display.setCursor(0,0);             // Start at top-left corner
  display.println(F("Hello, world!"));

  display.setTextColor(SSD1306_BLACK, SSD1306_WHITE); // Draw 'inverse' text
  display.println(3.141592);

  display.setTextSize(2);             // Draw 2X-scale text
  display.setTextColor(SSD1306_WHITE);
  display.print(F("0x")); display.println(0xDEADBEEF, HEX);

  display.display();
  delay(2000);
}

void testscrolltext(void) {
  display.clearDisplay();

  display.setTextSize(2); // Draw 2X-scale text
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(10, 0);
  display.println(F("scroll"));
  display.display();      // Show initial text
  delay(100);

  // Scroll in various directions, pausing in-between:
  display.startscrollright(0x00, 0x0F);
  delay(2000);
  display.stopscroll();
  delay(1000);
  display.startscrollleft(0x00, 0x0F);
  delay(2000);
  display.stopscroll();
  delay(1000);
  display.startscrolldiagright(0x00, 0x07);
  delay(2000);
  display.startscrolldiagleft(0x00, 0x07);
  delay(2000);
  display.stopscroll();
  delay(1000);
}

void testdrawbitmap(void) {
  display.clearDisplay();

  display.drawBitmap(
    (display.width()  - LOGO_WIDTH ) / 2,
    (display.height() - LOGO_HEIGHT) / 2,
    logo_bmp, LOGO_WIDTH, LOGO_HEIGHT, 1);
  display.display();
  delay(1000);
}

#define XPOS   0 // Indexes into the 'icons' array in function below
#define YPOS   1
#define DELTAY 2

void testanimate(const uint8_t *bitmap, uint8_t w, uint8_t h) {
  int8_t f, icons[NUMFLAKES][3];

  // Initialize 'snowflake' positions
  for(f=0; f< NUMFLAKES; f++) {
    icons[f][XPOS]   = random(1 - LOGO_WIDTH, display.width());
    icons[f][YPOS]   = -LOGO_HEIGHT;
    icons[f][DELTAY] = random(1, 6);
    Serial.print(F("x: "));
    Serial.print(icons[f][XPOS], DEC);
    Serial.print(F(" y: "));
    Serial.print(icons[f][YPOS], DEC);
    Serial.print(F(" dy: "));
    Serial.println(icons[f][DELTAY], DEC);
  }

  for(;;) { // Loop forever...
    display.clearDisplay(); // Clear the display buffer

    // Draw each snowflake:
    for(f=0; f< NUMFLAKES; f++) {
      display.drawBitmap(icons[f][XPOS], icons[f][YPOS], bitmap, w, h, SSD1306_WHITE);
    }

    display.display(); // Show the display buffer on the screen
    delay(200);        // Pause for 1/10 second

    // Then update coordinates of each flake...
    for(f=0; f< NUMFLAKES; f++) {
      icons[f][YPOS] += icons[f][DELTAY];
      // If snowflake is off the bottom of the screen...
      if (icons[f][YPOS] >= display.height()) {
        // Reinitialize to a random position, just off the top
        icons[f][XPOS]   = random(1 - LOGO_WIDTH, display.width());
        icons[f][YPOS]   = -LOGO_HEIGHT;
        icons[f][DELTAY] = random(1, 6);
      }
    }
  }
}


void setup() {
  Serial.begin(9600);

  // Wait for display
  delay(500);

  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!display.begin(SSD1306_SWITCHCAPVCC)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
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
}

// ============================================================
//  Cute face animation states for solar drag racer
// ============================================================

// Face geometry constants
#define FACE_CX  64   // face center X
#define FACE_CY  30   // face center Y
#define EYE_LX   48   // left eye X
#define EYE_RX   80   // right eye X
#define EYE_Y    26   // eye Y center
#define MOUTH_Y  42   // mouth Y center

// --- Helper: draw rounded face outline ---
void drawFaceOutline() {
  display.drawRoundRect(14, 4, 100, 52, 12, SSD1306_WHITE);
}

// --- Helper: draw status bar text at bottom ---
void drawStatusBar(const __FlashStringHelper* label) {
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  // Center the label at the bottom
  display.setCursor(0, 57);
  display.print(label);
}

// --- Helper: draw a cute blush (two small circles on cheeks) ---
void drawBlush() {
  display.fillCircle(36, 38, 3, SSD1306_WHITE);
  display.fillCircle(92, 38, 3, SSD1306_WHITE);
}

// --- Helper: draw battery icon at right side of status bar ---
void drawBattery(int level) { // level 0-4
  int bx = 108, by = 56;
  display.drawRect(bx, by, 18, 8, SSD1306_WHITE);
  display.fillRect(bx + 18, by + 2, 2, 4, SSD1306_WHITE);
  for (int i = 0; i < level; i++) {
    display.fillRect(bx + 2 + i * 4, by + 2, 3, 4, SSD1306_WHITE);
  }
}

// ======================== CHARGING ========================
// Happy face with lightning bolt, battery fills up
void animCharging(int frame) {
  display.clearDisplay();
  drawFaceOutline();

  // Happy eyes (^ ^) - arcs
  int blink = (frame % 20 == 0); // blink every 20 frames
  if (blink) {
    // closed eyes: horizontal lines
    display.drawLine(EYE_LX - 6, EYE_Y, EYE_LX + 6, EYE_Y, SSD1306_WHITE);
    display.drawLine(EYE_RX - 6, EYE_Y, EYE_RX + 6, EYE_Y, SSD1306_WHITE);
  } else {
    // Happy ^ ^ eyes
    display.drawLine(EYE_LX - 5, EYE_Y + 2, EYE_LX, EYE_Y - 4, SSD1306_WHITE);
    display.drawLine(EYE_LX, EYE_Y - 4, EYE_LX + 5, EYE_Y + 2, SSD1306_WHITE);
    display.drawLine(EYE_RX - 5, EYE_Y + 2, EYE_RX, EYE_Y - 4, SSD1306_WHITE);
    display.drawLine(EYE_RX, EYE_Y - 4, EYE_RX + 5, EYE_Y + 2, SSD1306_WHITE);
  }

  // Cute smile
  display.drawLine(FACE_CX - 8, MOUTH_Y, FACE_CX - 3, MOUTH_Y + 4, SSD1306_WHITE);
  display.drawLine(FACE_CX - 3, MOUTH_Y + 4, FACE_CX + 3, MOUTH_Y + 4, SSD1306_WHITE);
  display.drawLine(FACE_CX + 3, MOUTH_Y + 4, FACE_CX + 8, MOUTH_Y, SSD1306_WHITE);

  drawBlush();

  // Lightning bolt (animated flash)
  if ((frame / 4) % 2 == 0) {
    display.fillTriangle(FACE_CX - 2, EYE_Y - 10, FACE_CX + 4, EYE_Y - 4, FACE_CX, EYE_Y - 4, SSD1306_WHITE);
    display.fillTriangle(FACE_CX, EYE_Y - 4, FACE_CX + 2, EYE_Y - 4, FACE_CX - 2, EYE_Y + 2, SSD1306_WHITE);
  }

  drawStatusBar(F("CHARGING"));
  drawBattery((frame / 8) % 5); // battery fills over time
  display.display();
}

// ======================== WAKEUP ========================
// Eyes opening from closed to wide — stretching
void animWakeup(int frame) {
  display.clearDisplay();
  drawFaceOutline();

  int openness = min(frame, 8); // eyes open over 8 frames

  if (openness < 3) {
    // closed eyes: lines
    display.drawLine(EYE_LX - 6, EYE_Y, EYE_LX + 6, EYE_Y, SSD1306_WHITE);
    display.drawLine(EYE_RX - 6, EYE_Y, EYE_RX + 6, EYE_Y, SSD1306_WHITE);
  } else if (openness < 6) {
    // half open
    display.drawRoundRect(EYE_LX - 5, EYE_Y - 2, 10, 5, 2, SSD1306_WHITE);
    display.drawRoundRect(EYE_RX - 5, EYE_Y - 2, 10, 5, 2, SSD1306_WHITE);
    display.fillCircle(EYE_LX, EYE_Y, 1, SSD1306_WHITE);
    display.fillCircle(EYE_RX, EYE_Y, 1, SSD1306_WHITE);
  } else {
    // fully open - big round eyes with pupils
    display.drawCircle(EYE_LX, EYE_Y, 7, SSD1306_WHITE);
    display.drawCircle(EYE_RX, EYE_Y, 7, SSD1306_WHITE);
    display.fillCircle(EYE_LX, EYE_Y, 3, SSD1306_WHITE);
    display.fillCircle(EYE_RX, EYE_Y, 3, SSD1306_WHITE);
    // sparkle
    display.drawPixel(EYE_LX + 2, EYE_Y - 2, SSD1306_BLACK);
    display.drawPixel(EYE_RX + 2, EYE_Y - 2, SSD1306_BLACK);
  }

  // Mouth: small 'o' yawn that becomes smile
  if (openness < 5) {
    display.drawCircle(FACE_CX, MOUTH_Y + 2, 3 + openness / 2, SSD1306_WHITE); // yawn
  } else {
    // smile
    display.drawLine(FACE_CX - 6, MOUTH_Y, FACE_CX - 2, MOUTH_Y + 3, SSD1306_WHITE);
    display.drawLine(FACE_CX - 2, MOUTH_Y + 3, FACE_CX + 2, MOUTH_Y + 3, SSD1306_WHITE);
    display.drawLine(FACE_CX + 2, MOUTH_Y + 3, FACE_CX + 6, MOUTH_Y, SSD1306_WHITE);
  }

  // zzz fading out
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
// Confident smile, determined eyes, steady
void animReady(int frame) {
  display.clearDisplay();
  drawFaceOutline();

  int blink = (frame % 30 == 0);

  if (blink) {
    display.drawLine(EYE_LX - 6, EYE_Y, EYE_LX + 6, EYE_Y, SSD1306_WHITE);
    display.drawLine(EYE_RX - 6, EYE_Y, EYE_RX + 6, EYE_Y, SSD1306_WHITE);
  } else {
    // Determined eyes: slightly flat on top
    display.drawLine(EYE_LX - 6, EYE_Y - 4, EYE_LX + 6, EYE_Y - 4, SSD1306_WHITE);
    display.drawCircle(EYE_LX, EYE_Y, 6, SSD1306_WHITE);
    display.fillCircle(EYE_LX, EYE_Y, 2, SSD1306_WHITE);
    display.drawLine(EYE_RX - 6, EYE_Y - 4, EYE_RX + 6, EYE_Y - 4, SSD1306_WHITE);
    display.drawCircle(EYE_RX, EYE_Y, 6, SSD1306_WHITE);
    display.fillCircle(EYE_RX, EYE_Y, 2, SSD1306_WHITE);
  }

  // Confident grin
  display.drawLine(FACE_CX - 10, MOUTH_Y, FACE_CX - 4, MOUTH_Y + 5, SSD1306_WHITE);
  display.drawLine(FACE_CX - 4, MOUTH_Y + 5, FACE_CX + 4, MOUTH_Y + 5, SSD1306_WHITE);
  display.drawLine(FACE_CX + 4, MOUTH_Y + 5, FACE_CX + 10, MOUTH_Y, SSD1306_WHITE);

  // Thumbs-up star sparkle
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
// Intense excited face, wide eyes, big grin, speed lines
void animTurbo(int frame) {
  display.clearDisplay();
  drawFaceOutline();

  // Big excited eyes with star sparkle
  display.drawCircle(EYE_LX, EYE_Y, 8, SSD1306_WHITE);
  display.drawCircle(EYE_RX, EYE_Y, 8, SSD1306_WHITE);
  display.fillCircle(EYE_LX, EYE_Y, 4, SSD1306_WHITE);
  display.fillCircle(EYE_RX, EYE_Y, 4, SSD1306_WHITE);
  // star highlight in eyes
  display.drawPixel(EYE_LX + 3, EYE_Y - 3, SSD1306_BLACK);
  display.drawPixel(EYE_RX + 3, EYE_Y - 3, SSD1306_BLACK);

  // Big open grin
  display.drawLine(FACE_CX - 14, MOUTH_Y - 1, FACE_CX - 6, MOUTH_Y + 6, SSD1306_WHITE);
  display.drawLine(FACE_CX - 6, MOUTH_Y + 6, FACE_CX + 6, MOUTH_Y + 6, SSD1306_WHITE);
  display.drawLine(FACE_CX + 6, MOUTH_Y + 6, FACE_CX + 14, MOUTH_Y - 1, SSD1306_WHITE);
  display.drawLine(FACE_CX - 14, MOUTH_Y - 1, FACE_CX + 14, MOUTH_Y - 1, SSD1306_WHITE);

  // Speed lines on left side (animated)
  int offset = frame % 6;
  for (int i = 0; i < 4; i++) {
    int y = 12 + i * 12 + offset;
    display.drawLine(0, y, 10, y, SSD1306_WHITE);
  }

  // Flame/exhaust on right
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
// Calm zen face, half-closed peaceful eyes, gentle smile, leaf
void animEco(int frame) {
  display.clearDisplay();
  drawFaceOutline();

  int blink = (frame % 25 == 0);

  if (blink) {
    display.drawLine(EYE_LX - 6, EYE_Y, EYE_LX + 6, EYE_Y, SSD1306_WHITE);
    display.drawLine(EYE_RX - 6, EYE_Y, EYE_RX + 6, EYE_Y, SSD1306_WHITE);
  } else {
    // Half-closed peaceful eyes (u u shape)
    display.drawLine(EYE_LX - 5, EYE_Y - 2, EYE_LX, EYE_Y + 2, SSD1306_WHITE);
    display.drawLine(EYE_LX, EYE_Y + 2, EYE_LX + 5, EYE_Y - 2, SSD1306_WHITE);
    display.drawLine(EYE_RX - 5, EYE_Y - 2, EYE_RX, EYE_Y + 2, SSD1306_WHITE);
    display.drawLine(EYE_RX, EYE_Y + 2, EYE_RX + 5, EYE_Y - 2, SSD1306_WHITE);
  }

  // Gentle smile
  display.drawLine(FACE_CX - 6, MOUTH_Y, FACE_CX - 2, MOUTH_Y + 3, SSD1306_WHITE);
  display.drawLine(FACE_CX - 2, MOUTH_Y + 3, FACE_CX + 2, MOUTH_Y + 3, SSD1306_WHITE);
  display.drawLine(FACE_CX + 2, MOUTH_Y + 3, FACE_CX + 6, MOUTH_Y, SSD1306_WHITE);

  // Floating leaf animation
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
// Worried face, sweat drop, thin shaky mouth
void animSurvival(int frame) {
  display.clearDisplay();
  drawFaceOutline();

  // Worried eyes: round with raised inner brows
  display.drawCircle(EYE_LX, EYE_Y, 6, SSD1306_WHITE);
  display.drawCircle(EYE_RX, EYE_Y, 6, SSD1306_WHITE);
  display.fillCircle(EYE_LX, EYE_Y + 1, 2, SSD1306_WHITE); // pupils looking down
  display.fillCircle(EYE_RX, EYE_Y + 1, 2, SSD1306_WHITE);
  // Worried brows (angled)
  display.drawLine(EYE_LX - 7, EYE_Y - 10, EYE_LX + 3, EYE_Y - 8, SSD1306_WHITE);
  display.drawLine(EYE_RX - 3, EYE_Y - 8, EYE_RX + 7, EYE_Y - 10, SSD1306_WHITE);

  // Wavy worried mouth
  int wobble = (frame % 4 < 2) ? 0 : 1;
  display.drawLine(FACE_CX - 8, MOUTH_Y + wobble, FACE_CX - 3, MOUTH_Y + 2 + wobble, SSD1306_WHITE);
  display.drawLine(FACE_CX - 3, MOUTH_Y + 2 + wobble, FACE_CX + 3, MOUTH_Y + wobble, SSD1306_WHITE);
  display.drawLine(FACE_CX + 3, MOUTH_Y + wobble, FACE_CX + 8, MOUTH_Y + 2 + wobble, SSD1306_WHITE);

  // Sweat drop
  int sweatY = 10 + (frame % 10);
  display.fillCircle(95, sweatY + 3, 2, SSD1306_WHITE);
  display.fillTriangle(93, sweatY + 2, 97, sweatY + 2, 95, sweatY - 2, SSD1306_WHITE);

  drawStatusBar(F("SURVIVAL"));
  drawBattery(1);
  display.display();
}

// ======================== SLEEP ========================
// Closed eyes, zzz floating, peaceful
void animSleep(int frame) {
  display.clearDisplay();
  drawFaceOutline();

  // Closed eyes: gentle curves
  display.drawLine(EYE_LX - 5, EYE_Y, EYE_LX, EYE_Y + 2, SSD1306_WHITE);
  display.drawLine(EYE_LX, EYE_Y + 2, EYE_LX + 5, EYE_Y, SSD1306_WHITE);
  display.drawLine(EYE_RX - 5, EYE_Y, EYE_RX, EYE_Y + 2, SSD1306_WHITE);
  display.drawLine(EYE_RX, EYE_Y + 2, EYE_RX + 5, EYE_Y, SSD1306_WHITE);

  // Tiny peaceful mouth
  display.drawLine(FACE_CX - 3, MOUTH_Y + 2, FACE_CX + 3, MOUTH_Y + 2, SSD1306_WHITE);

  drawBlush();

  // Floating zzz animation
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

// ============================================================
//  State machine for demo loop
// ============================================================
enum RacerState { ST_CHARGING, ST_WAKEUP, ST_READY, ST_TURBO, ST_ECO, ST_SURVIVAL, ST_SLEEP, ST_COUNT };

RacerState currentState = ST_CHARGING;
int stateFrame = 0;
unsigned long lastFrameTime = 0;

#define FRAME_MS      80   // ~12.5 fps
#define STATE_FRAMES 60    // ~5 seconds per state

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

void loop() {
  unsigned long now = millis();
  if (now - lastFrameTime < FRAME_MS) return;
  lastFrameTime = now;

  if ((stateFrame % 12) == 0) {
    stabilizeDisplayController();
  }

  switch (currentState) {
    case ST_CHARGING: animCharging(stateFrame);  break;
    case ST_WAKEUP:   animWakeup(stateFrame);    break;
    case ST_READY:    animReady(stateFrame);      break;
    case ST_TURBO:    animTurbo(stateFrame);      break;
    case ST_ECO:      animEco(stateFrame);        break;
    case ST_SURVIVAL: animSurvival(stateFrame);   break;
    case ST_SLEEP:    animSleep(stateFrame);      break;
    default: break;
  }

  stateFrame++;
  if (stateFrame >= STATE_FRAMES) {
    stateFrame = 0;
    currentState = (RacerState)((currentState + 1) % ST_COUNT);
    Serial.print(F("State -> "));
    Serial.println(currentState);
  }
}