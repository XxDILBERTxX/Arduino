#include <Wire.h>
#include <U8g2lib.h>
#include "RoboEye_U8G2.h"

U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2_left(U8G2_R0, U8X8_PIN_NONE);
U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2_right(U8G2_R0, U8X8_PIN_NONE);

RoboEye_U8G2 leftEye(u8g2_left, false);
RoboEye_U8G2 rightEye(u8g2_right, true);

void setup() {
  Wire.begin();
  Wire.beginTransmission(0x3C); Wire.endTransmission();
  Wire.beginTransmission(0x3D); Wire.endTransmission();

  u8g2_left.setI2CAddress(0x3C << 1);
  u8g2_right.setI2CAddress(0x3D << 1);

  leftEye.begin();
  rightEye.begin();
}

void loop() {
  float x = sin(millis() / 1000.0);
  float y = cos(millis() / 1300.0);

  leftEye.setTarget(x, y);
  rightEye.setTarget(x, y); // parallel motion

  leftEye.update();
  rightEye.update();

  //delay(33);
}
