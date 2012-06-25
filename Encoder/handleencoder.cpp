
#define LIBCALL_PINCHANGEINT // keep it from double-defining things - annoying.
#include <PinChangeInt.h>
#include "handleencoder.h"


/* The pattern goes like this, with values (A, B)
 (0,0), (1,0), (1,1), (0,1)
 
 Keep track of what it was last time, and do something to counter
 (add one, subtract one, or do nothing if the same which should never happen)
 */


volatile EncoderType previous;
volatile bool lastA;
volatile bool lastB;
volatile EncoderType counter = 0;

typedef int8_t PositionType;

inline PositionType encodeSequencePosition(bool Aval, bool Bval) {
  if (Bval) {
    if (Aval) {
      return 0;
    } 
    else {
      return 1;
    }
  } 
  else { // bval is 1
    if (Aval) {
      return 2;
    } 
    else {
      return 3;
    }
  }
}


void encoderChanged() {
  if (PCintPort::arduinoPin == pinA) {
    lastA = PCintPort::pinState;
  } 
  else {
    lastB = PCintPort::pinState;
  }

  PositionType current = encodeSequencePosition(lastA, lastB);
  PositionType diff = current - previous; //Difference is the difference between the two
  if (diff < -1) {
    diff = diff + 4;
  } 
  else  if (diff > 1) {
    diff = diff - 4;
  }

  counter = counter + diff;
  previous = current;
}

void setupEncoder() {
  // input pins
  pinMode(pinA, INPUT);
  pinMode(pinB, INPUT);
  lastA = digitalRead(pinA);
  lastB = digitalRead(pinB);
  previous = encodeSequencePosition(lastA, lastB);
  // attach interrupt to the two pins
  PCintPort::attachInterrupt(pinA, encoderChanged, CHANGE);
  PCintPort::attachInterrupt(pinB, encoderChanged, CHANGE);
}


EncoderType getEncoderValue() {
  return counter; 
}

