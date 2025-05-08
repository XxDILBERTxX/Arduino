#include "RoboEye_U8G2.h"


RoboEye_U8G2::RoboEye_U8G2(U8G2 &display, bool isRight)
  : u8g2(display), isRightEye(isRight) {}


void RoboEye_U8G2::begin() {
  u8g2.begin();
}


void RoboEye_U8G2::setTarget(float normX, float normY) {
  normX = constrain(normX, -1.0, 1.0);
  normY = constrain(normY, -1.0, 1.0);

  // Move the full eye within screen bounds
  targetX = normX * (64 - eyeWidth);
  targetY = normY * (32 - eyeHeight);
}


void RoboEye_U8G2::update() {
  currentX += (targetX - currentX) * 0.5;
  currentY += (targetY - currentY) * 0.5;

  // Calculate where the eye will be on screen
  lookX = 64 + currentX;
  lookY = 32 + currentY;

  // Prevent drawing outside the screen
  lookX = constrain(lookX, eyeWidth, 128 - eyeWidth);
  lookY = constrain(lookY, eyeHeight, 64 - eyeHeight);

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
  u8g2.drawFilledEllipse(lookX, lookY, eyeWidth, eyeHeight);
  u8g2.setDrawColor(0);
  if (mad) {
    if (isRightEye) u8g2.drawTriangle(lookX - eyeWidth, lookY + eyeHeight, lookX + eyeWidth, lookY - eyeHeight,0,0);
    else u8g2.drawTriangle(lookX + eyeWidth, lookY + eyeHeight, lookX - eyeWidth, lookY - eyeHeight, 128, 0);
  }
  drawPupil();
  drawLids();
  u8g2.sendBuffer();
}


void RoboEye_U8G2::drawPupil() {
  u8g2.setDrawColor(0);

  // Calculate constrained offsets relative to the center of the eye
  float maxOffsetX = eyeWidth - pupilWidth;
  float maxOffsetY = eyeHeight - pupilHeight;

  float pupilOffsetX = constrain(currentX, -maxOffsetX, maxOffsetX);
  float pupilOffsetY = constrain(currentY, -maxOffsetY, maxOffsetY);

  u8g2.drawFilledEllipse(lookX + pupilOffsetX, lookY + pupilOffsetY, pupilWidth, pupilHeight);
}


void RoboEye_U8G2::drawLids() {
  if (!blinking) return;

  u8g2.setDrawColor(0);
  int lidAmount = (blinkFrame <= 5) ? blinkFrame : (10 - blinkFrame);
  lidAmount = map(lidAmount, 0, 5, 0, eyeHeight + pupilHeight);

  int top = max(0, lookY - lidAmount);
  int bottom = min(64, lookY + lidAmount);

  u8g2.drawBox(0, 0, 128, top);
  u8g2.drawBox(0, bottom, 128, 64 - bottom);

}
