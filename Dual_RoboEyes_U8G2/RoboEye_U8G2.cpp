#include "RoboEye_U8G2.h"

RoboEye_U8G2::RoboEye_U8G2(U8G2 &display, bool isRight)
  : u8g2(display), isRightEye(isRight) {}

void RoboEye_U8G2::begin() {
  u8g2.begin();
}

void RoboEye_U8G2::setTarget(float normX, float normY) {
  normX = constrain(normX, -1.0, 1.0);
  normY = constrain(normY, -0.9, 0.9);
  targetX = normX * (eyeWidth - pupilWidth - 3);
  targetY = normY * (eyeHeight - pupilHeight - 3);
}

void RoboEye_U8G2::update() {
  currentX += (targetX - currentX) * 0.5;
  currentY += (targetY - currentY) * 0.1;

  updateBlink();
  drawEye();
}

void RoboEye_U8G2::updateBlink() {
  unsigned long now = millis();

  if (!blinking && now - lastBlink > 8000) {
    blinking = true;
    blinkFrame = 0;
    lastBlink = now;
  }

  if (blinking) {
    blinkFrame++;
    if (blinkFrame >= 10) {
      blinking = false;
    }
  }
}

void RoboEye_U8G2::drawEye() {
  u8g2.clearBuffer();
  u8g2.setDrawColor(1);
  u8g2.drawFilledEllipse(eyeCenterX, eyeCenterY, eyeWidth, eyeHeight);
  drawPupil((int)(eyeCenterX + currentX), (int)(eyeCenterY + currentY));
  drawLids();
  u8g2.sendBuffer();
}

void RoboEye_U8G2::drawPupil(int tx, int ty) {
  u8g2.setDrawColor(0);
  u8g2.drawFilledEllipse(tx, ty, pupilWidth, pupilHeight);
  //u8g2.setFont(u8g2_font_unifont_t_symbols);
  //u8g2.drawGlyph(tx, ty, 0x2603);	
}

void RoboEye_U8G2::drawLids() {
  if (!blinking) return;

  u8g2.setDrawColor(0);
  int lidAmount = (blinkFrame <= 5) ? blinkFrame : (10 - blinkFrame);
  lidAmount = map(lidAmount, 0, 10, 0, eyeHeight);

  u8g2.drawBox(0, 0, 128, eyeCenterY + currentY - lidAmount);
  u8g2.drawBox(0, eyeCenterY + currentY + lidAmount, 128, 64 - (eyeCenterY + currentY + lidAmount));
}
