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
#include "Nunchuk.h"
#include "wiimote.h"

// Library/third-party includes
// - none

// Standard includes
// - none


Nunchuk::CalibData Nunchuk::calibration = {
	{ {0, 0, 0},
	{0, 0, 0x20 /*arbitrary choice*/},
	{
		{0xff, 0x00, 0x7f},
		{0xff, 0x00, 0x7f}
	}
}
};

static const uint8_t NunchukID[] = {0x00, 0x00, 0xA4, 0x20, 0x00, 0x00};

const Nunchuk::DataReport defaultReport = {
	{ 0x7f, 0x7f },
	{ 0, 0, 0x07 /*arbitrary choice*/ },
	0,
	0,
	0,
	false,
	false
};

Nunchuk * Nunchuk::self = 0;

Nunchuk::Nunchuk(callbackFnPtr yourFunction)
	: userFunction(yourFunction)
{
	self = this;
};

void Nunchuk::begin(Nunchuk::DataReport const & initialData) {
	typedef void (*voidFuncPtr)(void);
	voidFuncPtr callback = NULL;
	if (userFunction) {
		callback = &Nunchuk::trampoline;
	}
	wm_init(&(NunchukID[0]), const_cast<uint8_t *>(&(initialData.rawBytes[0])), &(calibration.rawBytes[0]), callback);
}
		
void Nunchuk::sendChange(Nunchuk::DataReport const & data) {
	wm_newaction(const_cast<uint8_t *>(&(data.rawBytes[0])));
}

void Nunchuk::trampoline() {
	self->userFunction(&self);
}