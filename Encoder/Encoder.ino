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
inline void startExtension() {
  nunchuk.begin(handleRequest);
  digitalWrite(LED_BUILTIN, HIGH);
}

void loop() {
  // code that should be repeated goes here  

  sendChange(nunchuk);

  if (checkVCC.hasExpired()) {
    int val = analogRead(WiimoteVCCPin);

    Serial << "Reading VCC wiimote sense: " << val << endl;
    if (connected && val < stopThreshold) {
      Serial << "Looks like we got unplugged." << endl;
      connected = false;  
      Nunchuk::setDeviceDetectLow();
    } 
    else if (!connected && val > startThreshold) {
      startExtension();
      Serial << "Probably connected!" << endl;
      connected = true;
    }
    checkVCC.reset();
  }

  if (dumpMapStatus.hasExpired()) {
    counterRange.process(getEncoderValue());
    counterRange.dumpStatus(Serial);
    dumpMapStatus.reset();
  }
  digitalWrite(LED_BUILTIN, LOW);
  sendChange(nunchuk);

  delay(250);
  //Serial << "Counter value: " << _DEC(myCounter) << " Mapped: " << _DEC(mapped) << endl;
  //Serial.println(mapcounter, DEC);
  Serial.println(getEncoderValue(), DEC);
  //report.joystickAxes[0] = lowByte(counter);
}
















