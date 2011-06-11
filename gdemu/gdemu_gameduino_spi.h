/**
 * GameduinoSPIClass
 * $Id$
 * \file gdemu_gameduino_spi.h
 * \brief GameduinoSPIClass
 * \date 2011-05-29 19:47GMT
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

#ifndef GDEMU_GAMEDUINO_SPI_H
#define GDEMU_GAMEDUINO_SPI_H
// #include <...>

// System includes

// Project includes
#include "wiring.h"

namespace GDEMU {

/**
 * GameduinoSPIClass
 * \brief GameduinoSPIClass
 * \date 2011-05-29 19:47GMT
 * \author Jan Boon (Kaetemi)
 */
class GameduinoSPIClass
{
public:
	GameduinoSPIClass() { }

	static void begin();
	static void end();

	static bool isSlaveSelected();
	static uint8_t getSSPin();
	static void slaveSelectChanged(uint8_t val);
	static uint8_t transfer(uint8_t data);

	static void setVBlank(uint8_t value); // internal emu use only
	static uint8_t getJ1Reset(); // internal emu use only

	static short getNextSampleL(); // internal emu use only
	static short getNextSampleR(); // internal emu use only

	static bool isScreenshotRequested(); // internal emu use only

	static uint8_t readRam(int offset); // duino loop use only
	static void writeRam(int offset, uint8_t value); // duino loop use only

	static short readRam16(int offset); // j1 loop use only
	static void writeRam16(int offset, short value); // j1 loop use only

	static uint8_t *getRam();

private:
	GameduinoSPIClass(const GameduinoSPIClass &);
	GameduinoSPIClass &operator=(const GameduinoSPIClass &);
	
}; /* class GameduinoSPIClass */

extern GameduinoSPIClass GameduinoSPI;

} /* namespace GDEMU */

#endif /* #ifndef GDEMU_GAMEDUINO_SPI_H */

/* end of file */
