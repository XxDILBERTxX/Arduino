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
  // Smoothly move pupil toward the full target
  currentX += (targetX - currentX) * 0.5;
  currentY += (targetY - currentY) * 0.5;

  // Let the eye white follow the pupil directly, but with spring dynamics
  const float stiffness = 0.1;
  const float damping   = 0.75;

  float springX = currentX - eyeFollowX;
  float springY = currentY - eyeFollowY;

  eyeVelX = eyeVelX * damping + springX * stiffness;
  eyeVelY = eyeVelY * damping + springY * stiffness;

  eyeFollowX += eyeVelX;
  eyeFollowY += eyeVelY;

  // Calculate where the eye white will be on screen
  lookX = 64 + eyeFollowX;
  lookY = 32 + eyeFollowY;

  // Clamp to screen bounds
  lookX = constrain(lookX, eyeWidth / 2, 128 - eyeWidth / 2);
  lookY = constrain(lookY, eyeHeight / 2, 64 - eyeHeight / 2);

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

  int topY = lookY - lidAmount;
  int bottomY = lookY + lidAmount;

  // Clip so we don't draw outside the screen
  topY = max(0, topY);
  bottomY = min(64, bottomY);

  u8g2.drawBox(0, 0, 128, topY);               // Top lid
  u8g2.drawBox(0, bottomY, 128, 64 - bottomY); // Bottom lid
}
