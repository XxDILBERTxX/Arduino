// My Ping Tank

//Motor left
#define PWMA 9 //Speed control
#define AIN1 8 //motor
#define AIN2 11 //motor
#define MOTOR_L 1 //Left motor as 1,
//Motor right
#define PWMB 10 //Speed control
#define BIN1 12 //motor
#define BIN2 13 //motor
#define MOTOR_R 2 //Right motor right as 2,

//others
#define STBPin 14 //Standby
#define pingPinR 5 // Pins of sensor
#define pingPinL 6 // Pins of sensor
long durationL, inchesL, durationR, inchesR;
int pingpins[] = {5,6};
int stbcs = LOW;
int stbls = LOW;
int stb;

void setup(){
	pinMode(STBPin, INPUT);
	pinMode(PWMA, OUTPUT);
	pinMode(AIN1, OUTPUT);
	pinMode(AIN2, OUTPUT);
	pinMode(PWMB, OUTPUT);
	pinMode(BIN1, OUTPUT);
	pinMode(BIN2, OUTPUT);
	//Serial.begin(9600);
}

void loop(){

	if(ping(1) >= 15 && ping(0) >= 15){
		move(MOTOR_L, 245, 0); //Motor left,
		move(MOTOR_R, 245, 0); //Motor right,
	}
	else if(ping(1) < 4 || ping(0) < 4){
		move(MOTOR_L, 200, 1);
		move(MOTOR_R, 200, 1);
	}
	else if(ping(0) < 7 && ping(1) >= 7){
		move(MOTOR_L, 230, 1);
		move(MOTOR_R, 230, 0);
	}
	else if(ping(1) < 7 && ping(0) >= 7){
		move(MOTOR_L, 230, 0);
		move(MOTOR_R, 230, 1);

	}
	
	stbcs = digitalRead(STBPin);
	if(stbcs == HIGH && stbls == LOW){
		delay(1);
		if(stb == HIGH){
			stb = LOW;
			}else{
			stb = HIGH;
		}
	}
	stbls = stbcs;
	
}

void move(int motor, int speed, int direction){
	boolean inPin1 = LOW;
	boolean inPin2 = HIGH;
	
	if(stb == LOW){ speed = LOW;} //Overide
	
	if(direction == 1){
		inPin1 = HIGH;
		inPin2 = LOW;
	}

	if(motor == 1){
		digitalWrite(AIN1, inPin1);
		digitalWrite(AIN2, inPin2);
		analogWrite(PWMA, speed);
	}
	else{
		digitalWrite(BIN1, inPin1);
		digitalWrite(BIN2, inPin2);
		analogWrite(PWMB, speed);
	}
}

//Delayed Off
//if(digitalRead(SENSOR_R)==LOW) TimerR = millis();
//  if(millis() - TimerR <= 1000UL) sensors +=1;
//if(digitalRead(SENSOR_L)==LOW) TimerL = millis();
//  if(millis() - TimerL <= 1000UL) sensors +=2;

unsigned long ping(int i){
	unsigned long echo;
	for(int i=0; i < 1; i++){
		pinMode(pingpins[i], OUTPUT); // Switch signalpin to output
		digitalWrite(pingpins[i], LOW); // Send low pulse
		delayMicroseconds(2); // Wait for 2 microseconds
		digitalWrite(pingpins[i], HIGH); // Send high pulse
		delayMicroseconds(5); // Wait for 5 microseconds
		digitalWrite(pingpins[i], LOW); // Holdoff
		pinMode(pingpins[i], INPUT); // Switch signalpin to input
		digitalWrite(pingpins[i], HIGH); // Turn on pullup resistor
		echo = pulseIn(pingpins[i], HIGH); //Listen for echo
		return (echo / 58.138) * .39; //convert to CM then to inches
		delay(10);
	}
}