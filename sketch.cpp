#include <arduino.h>
#include <PinChangeInt.h>


enum {
  pinI = 3,
  pinA = 4,
  pinB = 5
};

int counter = 0;

/* The pattern goes like this, with values (A, B)
  (0,0), (1,0), (1,1), (0,1)

  Keep track of what it was last time, and do something to counter
  (add one, subtract one, or do nothing if the same which should never happen)
*/
void encoderChanged() {


}

void setup(){
 // setup code goes here
 //Initialize serial
 Serial.begin(9600);
 // input pins
  pinMode(pinI, INPUT);
  pinMode(pinA, INPUT);
  pinMode(pinB, INPUT);

  // get first reading
  //PCintPort::attachInterrupt(2, &quicfunc, CHANGE);
}



void loop() {
 // code that should be repeated goes here


 int Ival = digitalRead(pinI);
 int Aval = digitalRead(pinA);
 int Bval = digitalRead(pinB);





 //Serial Write

 Serial.write ("I =  ");
 Serial.println (Ival,DEC);
 Serial.write ("A =  ");
 Serial.println (Aval,DEC);
 Serial.write ("B =  ");
 Serial.println (Bval,DEC);
 Serial.println();
 delay(500);
}
