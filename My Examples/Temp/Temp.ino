//
const int B=4275;                 // B value of the thermistor
const int R0 = 100000;            // R0 = 100k
const int pinTempSensor = A0;     // Grove - Temperature Sensor connect to A5

const int tempcheck = 5;
float temperatureC = 0 ;
float temperatureF = 0;
float temp[tempcheck];
float oldtemp = 0;

void setup(){
	delay(1000);
	Serial.begin(9600);
}

void loop(){
	
	int a = analogRead(pinTempSensor );
	float R = 1023.0/((float)a)-1.0;
	R = 100000.0*R;
	R = 1.0/(log(R/100000.0)/B+1/298.15)-273.15;//convert to temperature via datasheet;
	R = ((R * 9) / 5) + 32;//convert temperature to F
	for(int i = tempcheck; i > 0; i--)
	{
		temp[i] = temp[i - 1];
	}
	temp[0] = R;
	
	temperatureF = average(temp, tempcheck);
	int val = map(temperatureF, 60, 90, 0, 32);
	int val2 = map(temperatureF, 60, 90, 0, 255);
	val = constrain(val, 0 , 32);
	val2 = constrain(val2, 0 , 255);
	
	if (temperatureF != oldtemp) {
		//00.0000000000 = 2.10
		
		//
		Serial.print("Temp: F ");
		Serial.print(temperatureF,DEC);
		if (temperatureF < oldtemp){
			Serial.print(" - ");
		}
		else{
			Serial.print(" + ");
		}
		Serial.println();
		//
		
		oldtemp = temperatureF;
	}
	delay(100);
	
}

float average (float * array, int len)  // assuming array is int.
{
	float sum = 0 ;  // sum will be larger than an item, long for safety.
	for (int i = 0 ; i < len ; i++)
	sum += array [i] ;
	return  (sum / len);  // average will be fractional, so float may be appropriate.

}