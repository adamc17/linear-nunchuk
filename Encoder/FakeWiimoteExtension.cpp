/** @file
	@brief Implementation

	@date 2012

	@author
	Ryan Pavlik
	<rpavlik@iastate.edu> and <abiryan@ryand.net>
	http://academic.cleardefinition.com/
	Iowa State University Virtual Reality Applications Center
	Human-Computer Interaction Graduate Program

*/

//           Copyright Iowa State University 2012.
//  Distributed under the Boost Software License, Version 1.0.
//     (See accompanying file LICENSE_1_0.txt or copy at
//           http://www.boost.org/LICENSE_1_0.txt)

// Internal Includes
#include "FakeWiimoteExtension.h"

// Library/third-party includes
#include <Arduino.h>
#include <Wire.h>

// Standard includes
// - none


const FakeNunchuk::CalibData FakeNunchuk::calib = { {0, 0, 0},
	{0, 0, 0x20 /*arbitrary choice*/},
	{
		{0xff, 0x00, 0x7f},
		{0xff, 0x00, 0x7f}
	}
};

static const uint8_t NunchuckID[] = {0x00, 0x00, 0xA4, 0x20, 0x00, 0x00};

static const FakeNunchuk::DataReport initialData = {
	{ 0x7f, 0x7f },
	{ 0, 0, 0x07 /*arbitrary choice*/ },
	0,
	0,
	0,
	false,
	false
};
FakeNunchuk * FakeNunchuk::self = NULL;
FakeNunchuk::FakeNunchuk()
	: report(initialData)
	, addr(IDRegister)
	, receivedBytes(0)
	, gotRequestEvent(0)
        , gotReceiveEvent(0)
        , gotWriteData(0)
	, encrypted(false)
	, sentID(false) {
	memcpy(&report, &initialData, sizeof(report));
	memset(memorySpace, 0, sizeof(memorySpace));
	//memcpy(getMemoryLocation(OutputData), &initialData, sizeof(report));
	applyReportUpdates();
	memcpy(getMemoryLocation(IDRegister), &(NunchuckID[0]), sizeof(NunchuckID));
	memcpy(getMemoryLocation(CalibrationRegister), &calib, sizeof(calib));
}
void FakeNunchuk::begin() {
	self = this;
	Wire.begin(NunchukAddr);
	/// increase rate of I2C comms
	enum {
		TWI_FREQ_NUNCHUCK = 400000L
	};
	TWBR = ((F_CPU / TWI_FREQ_NUNCHUCK) - 16) / 2;
	Wire.onReceive(&FakeNunchuk::_receiveTrampoline);
	Wire.onRequest(&FakeNunchuk::_requestTrampoline);
}
void FakeNunchuk::printReceiveData() {
	bool didPrint = false;

	if (gotRequestEvent > 0) {
		Serial.print("Got ");
		Serial.print(gotRequestEvent, DEC);
		Serial.print(" request event(s). ");
		gotRequestEvent = 0;
		didPrint = true;
	}

	if (gotReceiveEvent > 0) {
		Serial.print("Got ");
		Serial.print(gotReceiveEvent, DEC);
		Serial.print(" receive event(s). ");
                if (gotWriteData > 0) {
                   Serial.print(gotWriteData, DEC);
                  Serial.print(" contained more than one byte.");
                  gotWriteData = 0;
                }
		for (uint8_t i = 0; i < gotReceiveEvent; ++i) {

			Serial.print(addr_log[i], HEX);
			Serial.print(" ");
		}
		gotReceiveEvent = 0;
		didPrint = true;
	}
	if (receivedBytes > 0) {
		Serial.print("Received data: ");
		for (uint8_t i = 0; i < receivedBytes; ++i) {
			Serial.print(bytes[i], HEX);
		}
		receivedBytes = 0;
		didPrint = true;
	}
	if (didPrint) {
		Serial.println("");
	}
}
#define DEBUGSTATEMENT(X) Serial.print(X)
#define DEBUGSTATEMENT2(X, Y) Serial.print(X, Y)
#define DEBUGENDLINE() Serial.println("")

inline void FakeNunchuk::receiveEvent(int howMany) {
        DEBUGSTATEMENT("Address: ");
	addr = Wire.read();
        howMany--;
        DEBUGSTATEMENT2(addr, HEX);
        if(howMany == 0) {
          DEBUGSTATEMENT(" Preparing for Read.");
        } else {
          DEBUGSTATEMENT(" Write:");
        }
          
	uint8_t count = 0;
	while (count < howMany && count < bufSize) {
		char c = Wire.read();
            
		memcpy(&(bytes[count]), &c, 1);
          DEBUGSTATEMENT(" ");

        DEBUGSTATEMENT2(bytes[count], HEX);
		count++;
	}

          DEBUGENDLINE();

        
	switch (addr) {
		case OutputData:
		case IDRegister:
		case CalibrationRegister:
			// these just will tell us what to send on the next request
			if (count > 0) {
				Serial.println("Wrote data when not expected!");
			}
			break;
		case EncryptionRegister:
			if (count > 0) {
				switch (bytes[0]) {
					case EncryptionEnabled:
						Serial.println("encryption!");
						encrypted = true;
						break;
					case EncryptionDisabled:
						Serial.println("no encryption!");
						encrypted = false;
						break;
					default:
						Serial.println("unexpected encryption value");
				}
			}
			break;
                case SecondaryInitRegister:
			// these just will tell us what to send on the next request
			if (count > 0) {
                          if (bytes[0] == 0x0) {
						Serial.println("no encryption - init complete");
                            
                          }
}
break;
                    
                case 0x40:
						//Serial.println("encryption!");
						//encrypted = true;
                                                //*getMemoryLocation(EncryptionRegister) = EncryptionEnabled;
						break;
                  
		default:
			Serial.print(addr, HEX);
			Serial.println(" register unknown");
	}
	if (count > 0) {
                gotWriteData = true;
			Serial.print(addr, HEX);
			Serial.println(" tried to write.");
		memcpy(getMemoryLocation(addr), &(bytes[0]), count);
	}
	receivedBytes = count;
	addr_log[gotReceiveEvent] = addr;
	gotReceiveEvent++;
}

inline void FakeNunchuk::requestEvent() {
	uint8_t outbuf[256] = {0};
	const uint8_t * outPtr = getMemoryLocation(addr);
	uint8_t outLen = min(0xff - addr, 0x08);
        bool recognizedRegister = true;
	switch (addr) {
		default:
			Serial.print("reading addr ");
			Serial.println(addr, HEX);
  recognizedRegister = false;
			break;
		case IDRegister:
			outLen = sizeof(NunchuckID);
			break;
		case OutputData:
			outLen = sizeof(report);
			break;
		case CalibrationRegister:
			outLen = HandshakeLength /*sizeof(calib)*/;
			break;
	}
        if(recognizedRegister) {
	if (encrypted) {
		for (uint8_t i = 0; i < outLen; ++i) {
			Wire.write(encryptByte(outPtr[i]));
		}
	} else {
		Wire.write(outPtr, outLen);
	}
//addr += outLen;
        }
        
else
{
        uint8_t i = 0;
        do {
  	if (encrypted) {
                  Wire.write(encryptByte(*getMemoryLocation(addr)));
            } else {
                  Wire.write(*getMemoryLocation(addr));
          }
          addr++;
          i++;
        } while (addr < 0xff && i < 8);

}
	gotRequestEvent++;
}

void FakeNunchuk::_receiveTrampoline(int howMany) {
	self->receiveEvent(howMany);
}
void FakeNunchuk::_requestTrampoline() {
	self->requestEvent();
}
