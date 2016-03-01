#include <Ping.h>

#define speedPinLeft 10 //Speed Left
#define IN3 12
#define IN4 13
#define Left 1

#define speedPinRight 9 //Speed Right
#define IN1 8
#define IN2 11
#define Right 0

#define STBPin 4 //Override

#define PINGS 3
int pingLog[PINGS];
Ping pingL = Ping(5, 0, 0);
Ping pingR = Ping(6, 0, 0);


boolean stb = true;

unsigned long current_time = 0;
unsigned long last_touched_time = 0;
//unsigned long

void setup(){
	pinMode(STBPin, INPUT);
	pinMode(speedPinRight, OUTPUT);
	pinMode(IN1, OUTPUT);
	pinMode(IN2, OUTPUT);
	pinMode(speedPinLeft, OUTPUT);
	pinMode(IN3, OUTPUT);
	pinMode(IN4, OUTPUT);
	Serial.begin(115200);
}

int oldLeftDist = 0;
int oldRightdist = 0;
void loop(){
	current_time = millis();
	
	pingL.fire();
	int LeftDist = (oldLeftDist + pingL.inches())/2;
	oldLeftDist = LeftDist;
	
	pingR.fire();
	int RightDist= (oldRightdist+ pingR.inches())/2;
	oldRightdist = RightDist;
	
	if(digitalRead(STBPin) && current_time - last_touched_time > 500) //Override
	{
		last_touched_time = current_time;
		stb = !stb;
	}
		
	if(LeftDist >= 15 && RightDist >= 15){
		move(Left, 245, 0); //Motor left,
		move(Right, 245, 0); //Motor right,
		Serial.print(" | Forward ");
	}
	else if(LeftDist < 5 || RightDist < 5){
		move(Left, 200, 1);
		move(Right, 200, 1);
		Serial.print(" | Backward ");
	}
	else if(RightDist < 10 && LeftDist >= 7){
		move(Left, 230, 1);
		move(Right, 230, 0);
		Serial.print(" | Left Turn ");
	}
	else if(LeftDist < 10 && RightDist >= 7){
		move(Left, 230, 0);
		move(Right, 230, 1);
		Serial.print(" | Right Turn ");
	}
	
	//
	Serial.print(" | LeftDist: ");
	Serial.print(LeftDist);
	Serial.print(" | RightDist: ");
	Serial.print(RightDist);
	
	Serial.print(" | On: ");
	Serial.print(stb);
	
	
	Serial.println();
	Serial.println();
}

void move(int motor, int speed, int direction){
	boolean inPin1 = LOW;
	boolean inPin2 = HIGH;
	
	if(stb){speed = LOW;} //Override
	
	if(direction == 1){
		inPin1 = HIGH;
		inPin2 = LOW;
	}

	if(motor == 0){
		digitalWrite(IN1, inPin1);
		digitalWrite(IN2, inPin2);
		analogWrite(speedPinRight, speed);
	}
	else{
		digitalWrite(IN3, inPin1);
		digitalWrite(IN4, inPin2);
		analogWrite(speedPinLeft, speed);
	}
}

