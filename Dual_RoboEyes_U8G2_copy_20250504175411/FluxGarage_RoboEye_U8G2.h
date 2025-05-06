// FluxGarage_RoboEye_U8G2.h
#pragma once
#include <U8g2lib.h>
#include <Arduino.h>

class FluxGarage_RoboEye_U8G2 {
  public:
    FluxGarage_RoboEye_U8G2(U8G2 &display, bool mirror=false)
      : u8g2(display), mirror(mirror) {}

    void begin() {
      blinkDuration = 300;
      openLevel = 1.0;
      nextBlink = millis() + random(3000, 7000);
      blinkStart = 0;
    }

    void update() {
      unsigned long now = millis();

      if (now > nextBlink && blinkStart == 0) {
        blinkStart = now;
        nextBlink = now + random(4000, 8000);
      }

      if (blinkStart > 0) {
        float t = (now - blinkStart) / (float)blinkDuration;
        if (t < 0.5) {
          openLevel = 1.0 - t * 2;
        } else if (t < 1.0) {
          openLevel = (t - 0.5) * 2;
        } else {
          openLevel = 1.0;
          blinkStart = 0;
        }
      }

      irisAngle += 0.05;
      if (irisAngle > TWO_PI) irisAngle -= TWO_PI;

      drawEye();
    }

  private:
    U8G2 &u8g2;
    bool mirror;

    float irisAngle = 0;
    float openLevel = 1.0;
    unsigned long nextBlink;
    unsigned long blinkStart;
    unsigned long blinkDuration;

    void drawEye() {
      u8g2.clearBuffer();

      const int w = 128;
      const int h = 64;
      const int cx = w / 2;
      const int cy = h / 2;
      const int eyeRadiusX = 50;
      const int eyeRadiusY = 30;

      // Eyelid curve
      int lidY = cy - (int)(eyeRadiusY * openLevel);

      // Eyeball outline
      u8g2.drawEllipse(cx, cy, eyeRadiusX, eyeRadiusY, U8G2_DRAW_ALL);

      // Iris
      float irisX = cx + cos(irisAngle) * 10;
      float irisY = cy + sin(irisAngle) * 6;
      u8g2.drawDisc(irisX, irisY, 10);

      // Pupil
      u8g2.setDrawColor(0);
      u8g2.drawDisc(irisX, irisY, 4);
      u8g2.setDrawColor(1);

      // Highlight
      u8g2.drawDisc(irisX - 3, irisY - 3, 2);

      // Eyelid mask
      u8g2.setDrawColor(0);
      u8g2.drawBox(0, 0, w, cy - lidY);
      u8g2.drawBox(0, cy + (cy - lidY), w, h);
      u8g2.setDrawColor(1);

      // Mirror if needed
      if (mirror) {
        u8g2.sendBuffer(); // Send as-is
      } else {
        // Flip horizontally: brute-force render to buffer
        uint8_t temp[1024];
        memcpy(temp, u8g2.getBufferPtr(), 1024);
        for (int y = 0; y < 8; y++) {
          for (int x = 0; x < 128; x++) {
            u8g2.getBufferPtr()[y * 128 + x] = temp[y * 128 + (127 - x)];
          }
        }
        u8g2.sendBuffer();
      }
    }
};
