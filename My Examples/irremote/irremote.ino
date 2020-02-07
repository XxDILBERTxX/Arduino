#include "IRremote.h"

//Remote
IRrecv irrecv(3);
decode_results results;
unsigned long button_time = 0;
unsigned long last_button_time = 0;

void setup() {
  delay(3000);
  //Remote
  irrecv.enableIRIn();
}

void loop() {
  //Remote
  if (irrecv.decode(&results)) {
    remote();
    irrecv.resume();
  }
}

//Remote
void remote() {
  unsigned long button_pressed = results.value;
  button_time = millis();
  if (button_time - last_button_time > 250) {
    switch (button_pressed) {
      case 0xFD00FF:  //1
        break;
      case 0xFD807F:  //2
        break;
      case 0xFD40BF:  //3
        break;
      case 0xFD20DF:  //4
        break;
      case 0xFDA05F:  //5
        break;
      case 0xFD609F:  //6
        break;
      case 0xFD10EF:  //7
        break;
      case 0xFD906F:  //8
        break;
      case 0xFD50AF:  //9
        break;
      case 0xFDB04F:  //0
        break;
      case 0xFD30CF:  //*
        break;
      case 0xFD708F:  //#
        break;
      case 0xFD8877:  //Up
        break;
      case 0xFD9867:  //Down
        break;
      case 0xFDA857:  //Ok
        break;
      case 0xFD28D7:  //Left
        break;
      case 0xFD6897:  //Right
        break;
    }
    last_button_time = button_time;
  }
}
