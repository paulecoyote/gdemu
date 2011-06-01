/**
 * AudioDriverClass
 * $Id$
 * \file gdemu_audio_driver.h
 * \brief AudioDriverClass
 * \date 2011-05-29 19:38GMT
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

#ifndef GDEMU_AUDIO_DRIVER_H
#define GDEMU_AUDIO_DRIVER_H
// #include <...>

// System includes

// Project includes

namespace GDEMU {

/**
 * AudioDriverClass
 * \brief AudioDriverClass
 * \date 2011-05-29 19:38GMT
 * \author Jan Boon (Kaetemi)
 */
class AudioDriverClass
{
public:
	AudioDriverClass() { }
	
	static void begin();
	static bool update();
	static void end();

	static int getFrequency(); // for now because in wasapi i can't have it automagically resample :(

	static void beginBuffer(short **buffer, int *samples);
	static void endBuffer();

private:
	AudioDriverClass(const AudioDriverClass &);
	AudioDriverClass &operator=(const AudioDriverClass &);
	
}; /* class AudioDriverClass */

extern AudioDriverClass AudioDriver;

} /* namespace GDEMU */

#endif /* #ifndef GDEMU_AUDIO_DRIVER_H */

/* end of file */
