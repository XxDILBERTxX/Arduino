#include <Wire.h>
#include <U8g2lib.h>

U8G2_SSD1306_128X64_NONAME_F_HW_I2C leftDisplay(U8G2_R0, U8X8_PIN_NONE);
U8G2_SSD1306_128X64_NONAME_F_HW_I2C rightDisplay(U8G2_R0, U8X8_PIN_NONE);

const int eyeRadius = 28;
const int pupilRadius = 8;
const int centerX = 64;
const int centerY = 32;
float angle = 0;
bool blink = false;
unsigned long lastBlink = 0;
int blinkInterval = 3000;
int blinkDuration = 150;

void setup() {
  Wire.begin();

  leftDisplay.setI2CAddress(0x3C << 1);
  rightDisplay.setI2CAddress(0x3D << 1);
  leftDisplay.begin();
  rightDisplay.begin();
}

void drawEye(U8G2 &display, bool mirrored, float a, bool blinkNow) {
  int dx = (int)(eyeRadius * 0.5 * cos(a));
  int dy = (int)(eyeRadius * 0.4 * sin(a));
  if (mirrored) dx = -dx;

  display.clearBuffer();

  if (blinkNow) {
    // Draw closed eye (horizontal line)
    display.drawLine(centerX - eyeRadius, centerY, centerX + eyeRadius, centerY);
  } else {
    // Outer eye
    display.drawCircle(centerX, centerY, eyeRadius);
    // Pupil
    display.drawDisc(centerX + dx, centerY + dy, pupilRadius);
  }

  display.sendBuffer();
}

void loop() {
  unsigned long now = millis();

  // Blinking logic
  if (now - lastBlink > blinkInterval) {
    blink = true;
    if (now - lastBlink > blinkInterval + blinkDuration) {
      blink = false;
      lastBlink = now;
      blinkInterval = random(2500, 5000);
    }
  }

  // Move pupil in a circular path
  angle += 0.05;
  if (angle > TWO_PI) angle = 0;

  // Draw left and right eyes
  drawEye(leftDisplay, true, angle, blink);
  drawEye(rightDisplay, false, angle, blink);

  delay(30);
}
