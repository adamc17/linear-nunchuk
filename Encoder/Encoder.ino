#include <PinChangeInt.h>
#include "Nunchuk.h"


Nunchuk nunchuk;

//Declares Output Pins
enum {
	pinI = 3,
	pinA = 4,
	pinB = 5

};


/* The pattern goes like this, with values (A, B)
 (0,0), (1,0), (1,1), (0,1)

 Keep track of what it was last time, and do something to counter
 (add one, subtract one, or do nothing if the same which should never happen)
 */


volatile int previous;
volatile bool lastA;
volatile bool lastB;
volatile int counter = 0;

inline int encodeSequencePosition(bool Aval, bool Bval) {
	if (Bval == 0) {
		if (Aval == 0) {
			return 0;
		} else {
			return 1;
		}
	} else { // bval is 1
		if (Aval == 1) {
			return 2;
		} else {
			return 3;
		}
	}
}


void setupEncoder() {
	// input pins
	pinMode(pinA, INPUT);
	pinMode(pinB, INPUT);
	lastA = digitalRead(pinA);
	lastB = digitalRead(pinB);
	previous = encodeSequencePosition(lastA, lastB);
}

void encoderChanged() {
	if (PCintPort::arduinoPin == pinA) {
		lastA = PCintPort::pinState;
	} else {
		lastB = PCintPort::pinState;
	}

	int current = encodeSequencePosition(lastA, lastB);
	int diff = current - previous; //Difference is the difference between the two
	if (diff < -1) {
		diff = diff + 4;
	} else {
		if (diff > 1) {
			diff = diff - 4;
		}
	}

	counter = counter + diff;
	previous = current;
}

// initial values
Nunchuk::DataReport report = Nunchuk::defaultReport;

void setup() {
	// setup code goes here
	//Initialize serial
	Serial.begin(115200);
	// get first reading
	setupEncoder();
	// attach interrupt to the two pins
	PCintPort::attachInterrupt(pinA, encoderChanged, CHANGE);
	PCintPort::attachInterrupt(pinB, encoderChanged, CHANGE);

	nunchuk.begin();
}

int stride = 0;
void loop() {
	// code that should be repeated goes here

	float mapcounter = counter / 3600.0;
	delay(250);
	//Serial.println(mapcounter, DEC);
	//Serial.println(counter, DEC);
	report.joystickAxes[0] = lowByte(counter);
	nunchuk.sendChange(report);
}











