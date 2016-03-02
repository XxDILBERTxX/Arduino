#include <Ping.h>

#define speedPinLeft 10 //Speed Left
#define IN3 12
#define IN4 13
#define Left 0

#define speedPinRight 9 //Speed Right
#define IN1 8
#define IN2 11
#define Right 1

#define STBPin 4 //Override

#define DangerDist 5
#define WarningDist 10
#define ClearDist 15

Ping pingL = Ping(5, 4.2, 0);
Ping pingR = Ping(6, 1.1, 0);

int oldLeftDist = 0;
int oldRightdist = 0;

boolean stb = true;

unsigned long current_time = 0;
unsigned long last_touched_time = 0;

void setup(){
	pinMode(STBPin, INPUT);
	pinMode(speedPinRight, OUTPUT);
	pinMode(IN1, OUTPUT);
	pinMode(IN2, OUTPUT);
	pinMode(speedPinLeft, OUTPUT);
	pinMode(IN3, OUTPUT);
	pinMode(IN4, OUTPUT);
	//Serial.begin(115200);
}

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
	
	if(LeftDist < DangerDist || RightDist < DangerDist){
		move(Left, 128, 1);
		move(Right, 128, 1);
		//Serial.print(" | Backward ");
		//Serial.println();
	}
	else if(RightDist < WarningDist && LeftDist > DangerDist){
		move(Left, 128, 1);
		move(Right, 128, 0);
		//Serial.print(" | Left Turn ");
		//Serial.println();
	}
	else if(LeftDist < WarningDist && RightDist > DangerDist){
		move(Left, 128, 0);
		move(Right, 128, 1);
		//Serial.print(" | Right Turn ");
		//Serial.println();
	}
	
	else if(RightDist < ClearDist && LeftDist > WarningDist){
		move(Left, 64, 0);
		move(Right, 192, 0);
		//Serial.print(" | Left drift ");
		//Serial.println();
	}
	else if(LeftDist < ClearDist && RightDist > WarningDist){
		move(Left, 192, 0);
		move(Right, 64, 0);
		//Serial.print(" | Right drift ");
		//Serial.println();
	}
	
	else if(LeftDist >= ClearDist && RightDist >= ClearDist){
		move(Left, 128, 0); //Motor left,
		move(Right, 128, 0); //Motor right,
		//Serial.print(" | Forward ");
		//Serial.println();
	}
	
	// Break point / Debugging start
	
	// Stop
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
		digitalWrite(IN3, inPin1);
		digitalWrite(IN4, inPin2);
		analogWrite(speedPinLeft, speed);
		
	}
	if(motor == 1){
		digitalWrite(IN1, inPin1);
		digitalWrite(IN2, inPin2);
		analogWrite(speedPinRight, speed);
	}
	
}

