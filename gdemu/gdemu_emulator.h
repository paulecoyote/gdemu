/**
 * EmulatorClass
 * $Id$
 * \file gdemu_emulator.h
 * \brief EmulatorClass
 * \date 2011-05-29 19:54GMT
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

#ifndef GDEMU_EMULATOR_H
#define GDEMU_EMULATOR_H
// #include <...>

// System includes

// Project includes (include standard stuff for user)
#include "gdemu_audio_machine.h"
#include "gdemu_graphics_machine.h"
#include "gdemu_j1.h"
#include "gdemu_keyboard.h"

namespace GDEMU {

enum EmulatorFlags
{
	// enables the keyboard to be used as input
	EmulatorEnableKeyboard = 0x01, 
	// enables audio
	EmulatorEnableAudio = 0x02, 
	// enables j1
	EmulatorEnableJ1 = 0x04, 

	// causes J1 to run on the main graphics thread
	EmulatorSyncJ1GraphicsYLine = 0x10, 	
	// causes J1 to run on the audio thread
	EmulatorSyncJ1AudioSample = 0x20, 
	// combination of the two flags above, causes audio to sync with the graphics thread
	
	// links the FREQTICK register to audio sample playback, otherwise by system timer
	EmulatorFreqTickByAudioSample = 0x40, 
};

/**
 * EmulatorClass
 * \brief EmulatorClass
 * \date 2011-05-29 19:54GMT
 * \author Jan Boon (Kaetemi)
 */
class EmulatorClass
{
public:
	EmulatorClass() { }

	static void run(void (*setup)(), void (*loop)(), int flags);
	
private:
	EmulatorClass(const EmulatorClass &);
	EmulatorClass &operator=(const EmulatorClass &);
	
}; /* class EmulatorClass */

extern EmulatorClass Emulator;

} /* namespace GDEMU */

#endif /* #ifndef GDEMU_EMULATOR_H */

/* end of file */
