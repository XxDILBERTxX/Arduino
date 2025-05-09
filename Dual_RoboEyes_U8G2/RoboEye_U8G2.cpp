#ifndef ROBO_EYE_U8G2_H
#define ROBO_EYE_U8G2_H

#include <Arduino.h>
#include <U8g2lib.h>

class RoboEye_U8G2 {
public:
  RoboEye_U8G2(U8G2 &display, bool isRight);

  void begin();
  void setTarget(float normX, float normY);
  void update();
  
private:
  U8G2 &u8g2;
  bool isRightEye;

  int eyeCenterX = 64;
  int eyeCenterY = 32;
  int eyeWidth = 20;
  int eyeHeight = 10; 
  int pupilWidth = 4;
  int pupilHeight = 8;

  float targetX = 0;
  float targetY = 0;
  float currentX = 0;
  float currentY = 0;
  float eyeFollowX = 0;
  float eyeFollowY = 0;
  float lookX = 0;
  float lookY = 0;
  float eyeVelX = 0;
  float eyeVelY = 0;



  unsigned long lastBlink = 0;
  bool blinking = false;
  int blinkFrame = 0;

  bool mad = false;

  void updateBlink();
  void drawEye();
  void drawPupil();
  void drawLids();

};

#endif