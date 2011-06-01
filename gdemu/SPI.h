/**
 * SPIClass
 * $Id$
 * \file SPI.h
 * \brief SPIClass
 */

/* 
 * Copyright (C) 2011  by authors
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

#ifndef SPI_H
#define SPI_H

// System includes
#include <stdio.h>

// Project includes
#include "WProgram.h"
#include "avr/pgmspace.h"

// Defines
#define SPI_CLOCK_DIV4 0x00
#define SPI_CLOCK_DIV16 0x01
#define SPI_CLOCK_DIV64 0x02
#define SPI_CLOCK_DIV128 0x03
#define SPI_CLOCK_DIV2 0x04
#define SPI_CLOCK_DIV8 0x05
#define SPI_CLOCK_DIV32 0x06
#define SPI_CLOCK_DIV64 0x07

#define SPI_MODE0 0x00
#define SPI_MODE1 0x04
#define SPI_MODE2 0x08
#define SPI_MODE3 0x0C

#define SPI_MODE_MASK 0x0C  // CPOL = bit 3, CPHA = bit 2 on SPCR
#define SPI_CLOCK_MASK 0x03  // SPR1 = bit 1, SPR0 = bit 0 on SPCR
#define SPI_2XCLOCK_MASK 0x01  // SPI2X = bit 0 on SPSR

/**
 * SPIClass
 * \brief SPIClass
 */
class SPIClass
{
public:
	inline SPIClass() { }

	static byte transfer(byte _data);
	
	// SPI Configuration methods
	
	//static void attachInterrupt();
	//static void detachInterrupt(); // Default
	
	static void begin() { } // Default
	static void end() { }
	
	static void setBitOrder(uint8_t) { }
	static void setDataMode(uint8_t) { }
	static void setClockDivider(uint8_t) { }

private:
	SPIClass(const SPIClass &);
	SPIClass &operator=(const SPIClass &);
	
}; /* class SPI */

extern SPIClass SPI;

#endif /* #ifndef SPI_H */

/* end of file */
