/**
 * SystemClass
 * $Id$
 * \file gdemu_system.cpp
 * \brief SystemClass
 * \date 2011-05-25 19:32GMT
 * \author Jan Boon (Kaetemi)
 */

/* 
 * Copyright (C) 2011  Jan Boon (Kaetemi)
 * 
 * This file is part of GAMEDUINO EMULATOR.
 * GAMEDUINO EMULATOR is free software: you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation, either version 2 of the
 * License, or (at your option) any later version.
 * 
 * GAMEDUINO EMULATOR is distributed in the hope that it will be
 * useful, but WITHOUT ANY WARRANTY; without even the implied warranty
 * of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with GAMEDUINO EMULATOR; see the file COPYING.  If not, see
 * <http://www.gnu.org/licenses/>.
 */

// #include <...>
#include "gdemu_system.h"

// System includes

// Project includes

// using namespace ...;

namespace GDEMU {


#define D_GDEMU_FPS_SMOOTHING 36


double SystemClass::s_FrameTime, SystemClass::s_FrameTimeDelta; //, SystemClass::s_FrameTimeOffset;
int SystemClass::s_FrameCount;
double SystemClass::s_FPSSmooth;
bool SystemClass::s_MainThreadSwitchable = false;


static double s_FPSSmoothValues[D_GDEMU_FPS_SMOOTHING];
//static double s_FPSSmoothTotal;
static int s_FPSSmoothAt;
static int s_FPSSmoothCount;

static uint16_t s_AnalogPins[64];

void SystemClass::begin()
{
	SystemClass::_begin();

	// Initialize Frame Count
	s_FrameCount = 0;

	// Initialize Frame Timing Information
	s_FrameTime = 0.0;
	s_FrameTimeDelta = 0.0;
	s_FPSSmooth = 0.0;
	s_FPSSmoothAt = 0;
	for (int i = 0; i < D_GDEMU_FPS_SMOOTHING; ++i)
		s_FPSSmoothValues[i] = 0.0;
	//s_FPSSmoothTotal = 0.0;
	s_FPSSmoothCount = 0;
	//s_FrameTimeOffset = SystemClass::getSeconds() * -1.0;
}

void SystemClass::update()
{
	SystemClass::_update();

	// Update Frame Count
	++s_FrameCount;
	
	// Update Frame Timing Information
	double frameTime = getSeconds();
	s_FrameTimeDelta = frameTime - s_FrameTime;
	s_FrameTime = frameTime;

	//s_FPSSmoothTotal -= s_FPSSmoothValues[s_FPSSmoothAt];
	s_FPSSmoothValues[s_FPSSmoothAt] = frameTime; //getFPS();
	//s_FPSSmoothTotal += s_FPSSmoothValues[s_FPSSmoothAt];
	if (s_FPSSmoothCount > 0)
		s_FPSSmooth = (double)(s_FPSSmoothCount - 1) / (s_FPSSmoothValues[s_FPSSmoothAt] - s_FPSSmoothValues[(s_FPSSmoothAt + 1) % s_FPSSmoothCount]);
	++s_FPSSmoothAt;
	if (s_FPSSmoothCount < s_FPSSmoothAt)
		s_FPSSmoothCount = s_FPSSmoothAt;
	s_FPSSmoothAt %= D_GDEMU_FPS_SMOOTHING;
	//s_FPSSmooth = //s_FPSSmoothTotal / (double)s_FPSSmoothCount;
}

void SystemClass::end()
{
	// ...

	SystemClass::_end();
}

uint16_t SystemClass::getAnalogRead(uint8_t pin)
{
	return s_AnalogPins[pin];
}

void SystemClass::setAnalogRead(uint8_t pin, uint16_t value)
{
	s_AnalogPins[pin] = value;
}

} /* namespace GDEMU */

/* end of file */
