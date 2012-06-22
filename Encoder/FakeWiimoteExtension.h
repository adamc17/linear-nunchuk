/** @file
	@brief Header

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

#pragma once
#ifndef INCLUDED_FakeWiimoteExtension_h_GUID_900138B8_E901_4FB9_FBD5_706A5C94195C
#define INCLUDED_FakeWiimoteExtension_h_GUID_900138B8_E901_4FB9_FBD5_706A5C94195C


// Internal Includes
// - none

// Library/third-party includes
// - none

// Standard includes
#include <stdint.h>
#include <string.h> // for memcpy


class FakeNunchuk {
	public:
		enum {
			NunchukAddr = 0x52,
			ReplyLength = 6,
			HandshakeLength = 224, /* from wiiuse */
			EncryptionEnabled = 0xAA,
			EncryptionDisabled = 0x55
		};

		enum ExpansionRegisterAddress {
			OutputData = 0x00,
			IDRegister = 0xFA,
			CalibrationRegister = 0x20,
			EncryptionRegister = 0xF0,
                        SecondaryInitRegister = 0xFB
		};

		struct DataReport {
			uint8_t joystickAxes[2];
			uint8_t accelAxesHigh[3];
			unsigned int accelZLow : 2;
			unsigned int accelYLow : 2;
			unsigned int accelXLow : 2;
			bool buttonC : 1;
			bool buttonZ : 1;
		};
		struct CalibData {
			uint8_t accelCalibZero[3];
			uint8_t accelCalibG[3];
			struct JSCalibAxis {
				uint8_t maxVal;
				uint8_t minVal;
				uint8_t centerVal;
			} jsCalibAxes[2];
		};

		FakeNunchuk();

		void begin();

		void printReceiveData();

		uint8_t * getMemoryLocation(uint8_t loc) {
			return &(memorySpace[loc]);
		}

		void applyReportUpdates(){
	memcpy(getMemoryLocation(OutputData), &report, sizeof(report));
}
		DataReport report;

	private:
                bool initialized;
                void wipeMemory();
                void initializeMemory();
		static const uint8_t expansionID[6];
		static const CalibData calib;
		static const uint8_t bufSize = 32;
		uint8_t addr;
		uint8_t bytes[bufSize];
		uint8_t memorySpace[256];
		uint8_t receivedBytes;
		uint8_t gotReceiveEvent;
		uint8_t gotRequestEvent;
                uint8_t gotWriteData;

		uint8_t addr_log[256];
		void receiveEvent(int howMany);
		void requestEvent();


		uint8_t encryptByte(const uint8_t val) {
			return (val - 0x17) ^ 0x17;
		}

		uint8_t decryptByte(const uint8_t val) {
			return (val ^ 0x17) + 0x17;
		}

		bool encrypted;
		bool sentID;
		static FakeNunchuk * self;
		static void _receiveTrampoline(int howMany);
		static void _requestTrampoline();
};


#endif // INCLUDED_FakeWiimoteExtension_h_GUID_900138B8_E901_4FB9_FBD5_706A5C94195C

