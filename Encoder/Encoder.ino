#define NO_PORTB_PINCHANGES
#define NO_PORTC_PINCHANGES
#define DISABLE_PCINT_MULTI_SERVICE
#include <PinChangeInt.h>
#include <Streaming.h>
#include "AutoRanging.h"
#include "Watchdog.h"
#include "Nunchuk.h"


#include "handleencoder.h"


Nunchuk nunchuk;
// initial values
Nunchuk::DataReport report = Nunchuk::defaultReport;

Watchdog checkVCC(1000);

AutoRanging<int, uint8_t> counterRange(0, 255);
void sendChange(Nunchuk & n) {
  uint8_t mapped = counterRange.process(getEncoderValue());
  report.joystickAxes[0] = mapped;
  n.sendChange(report);
}

void handleRequest(Nunchuk & n) {
  sendChange(n);
  checkVCC.reset();
  digitalWrite(LED_BUILTIN, HIGH);
}


enum {
  WiimoteVCCPin = A0
};

void setup() {
  // setup code goes here
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);

  Nunchuk::setDeviceDetectLow();


  //Initialize serial
  Serial.begin(115200);
  // get first reading
  setupEncoder();
  Serial.print("Encoder setup complete, counter currently at ");
  Serial.println(getEncoderValue());
  counterRange.setCenter(getEncoderValue());
  checkVCC.reset();
}

Watchdog dumpMapStatus(5000);

enum {
  startThreshold = 500,
  stopThreshold = 300
};

bool connected = false;

void checkWiimoteStatus() {
  if (checkVCC.hasExpired()) {
    int val = analogRead(WiimoteVCCPin);
    if (connected && val < stopThreshold) {
      Serial << "Looks like we got unplugged." << endl;
      connected = false;  
      digitalWrite(LED_BUILTIN, LOW);
      Nunchuk::setDeviceDetectLow();
    } 
    else if (!connected && val > startThreshold) {
      Serial << "Probably connected!" << endl;
      connected = true;
      nunchuk.begin(handleRequest);
      digitalWrite(LED_BUILTIN, HIGH);
    }

    checkVCC.reset();
  }
}

void loop() {
  // code that should be repeated goes here  
  checkWiimoteStatus();
  if (connected) {
    sendChange(nunchuk);
  }



  if (dumpMapStatus.hasExpired()) {
    counterRange.process(getEncoderValue());
    counterRange.dumpStatus(Serial);
    dumpMapStatus.reset();
  }
  sendChange(nunchuk);

  delay(500);
  //Serial << "Counter value: " << _DEC(myCounter) << " Mapped: " << _DEC(mapped) << endl;
  //Serial.println(mapcounter, DEC);
  Serial.println(getEncoderValue(), DEC);
  //report.joystickAxes[0] = lowByte(counter);
}

















