#include "RoboEye_U8G2.h"

RoboEye_U8G2::RoboEye_U8G2(U8G2 &display, bool isRight)
  : u8g2(display), isRightEye(isRight) {}

void RoboEye_U8G2::begin() {
  u8g2.begin();
}

void RoboEye_U8G2::setTarget(float normX, float normY) {
  normX = constrain(normX, -1.0, 1.0);
  normY = constrain(normY, -1.0, 1.0);

  targetX = normX * (eyeWidth - pupilWidth - 2);
  targetY = normY * (pupilHeight - eyeHeight - 2);
}

void RoboEye_U8G2::update() {
  currentX += (targetX - currentX) * 0.5;
  currentY += (targetY - currentY) * 0.5;
  lookX = eyeCenterX + currentX;
  lookY = eyeCenterY + currentY;

  updateBlink();
  drawEye();
}

void RoboEye_U8G2::updateBlink() {
  unsigned long now = millis();

  if (!blinking && now - lastBlink > 7000) {
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
  u8g2.drawFilledEllipse(lookX, lookY, eyeWidth, eyeHeight);
  drawPupil();
  drawLids();
  u8g2.sendBuffer();
}

void RoboEye_U8G2::drawPupil() {
  u8g2.setDrawColor(0);
  u8g2.drawFilledEllipse(lookX + currentX, lookY + currentY, pupilWidth, pupilHeight);
}

void RoboEye_U8G2::drawLids() {
  if (!blinking) return;

  u8g2.setDrawColor(0);
  int lidAmount = (blinkFrame <= 5) ? blinkFrame : (10 - blinkFrame);
  lidAmount = map(lidAmount, 0, 5, 0, eyeHeight - pupilHeight - (pupilHeight /2));

  u8g2.drawBox(0, 0, 128, lookY + currentY - lidAmount);
  u8g2.drawBox(0, lookY + currentY + lidAmount, 128, 64 - (lookY + currentY + lidAmount));
}
