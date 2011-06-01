/**
 * SystemClass
 * $Id$
 * \file gdemu_system.h
 * \brief SystemClass
 * \date 2011-05-25 19:28GMT
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

#ifndef GDEMU_SYSTEM_H
#define GDEMU_SYSTEM_H
// #include <...>

// System includes

// Project includes
#include "wiring.h"

namespace GDEMU {

/**
 * SystemClass
 * \brief SystemClass
 * \date 2011-05-25 19:28GMT
 * \author Jan Boon (Kaetemi)
 */
class SystemClass
{
public:
	SystemClass() { }

	static void begin();
	static void update();
	static void end();
	
	// OS Specific
	static double getSeconds();
	static long getMillis();
	static long getMicros();
	
	static void delay(int ms);
	static void delayMicros(int us);

	static void disableAutomaticPriorityBoost();

	static void makeLowPriorityThread();
	static void makeNormalPriorityThread();
	static void makeHighPriorityThread();
	static void makeHighestPriorityThread();
	static void makeRealtimePriorityThread();
	
	static void makeDuinoThread();
	static bool isDuinoThread();
	static void prioritizeDuinoThread();
	static void unprioritizeDuinoThread();
	static void holdDuinoThread();
	static void resumeDuinoThread();

	static void switchThread();

	static void *setThreadGamesCategory(unsigned long *);
	static void revertThreadCategory(void *);

	// Duino
	static uint16_t getAnalogRead(uint8_t pin);
	static void setAnalogRead(uint8_t pin, uint16_t value);
	
	// Debugging
	static inline double getFrameTime() { return s_FrameTime; }
	static inline double getFrameTimeDelta() { return s_FrameTimeDelta; }

	static inline double getFPS() { return 1.0 / s_FrameTimeDelta; }
	static inline double getFPSSmooth() { return s_FPSSmooth; }
	static inline int getFrameCount() { return s_FrameCount; }

private:
	static void _begin();
	static void _update();
	static void _end();

private:
	static double s_FrameTime, s_FrameTimeDelta;
	static int s_FrameCount;
	static double s_FPSSmooth;

private:
	SystemClass(const SystemClass &);
	SystemClass &operator=(const SystemClass &);
	
}; /* class SystemClass */

extern SystemClass System;

} /* namespace GDEMU */

#endif /* #ifndef GDEMU_SYSTEM_H */

/* end of file */
